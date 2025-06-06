/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2015 Intel Corporation
 */

#ifndef _IXGBE_RXTX_VEC_COMMON_H_
#define _IXGBE_RXTX_VEC_COMMON_H_
#include <stdint.h>
#include <ethdev_driver.h>

#include "../common/rx.h"
#include "ixgbe_ethdev.h"
#include "ixgbe_rxtx.h"

static __rte_always_inline int
ixgbe_tx_free_bufs(struct ci_tx_queue *txq)
{
	struct ci_tx_entry_vec *txep;
	uint32_t status;
	uint32_t n;
	uint32_t i;
	int nb_free = 0;
	struct rte_mbuf *m, *free[RTE_IXGBE_TX_MAX_FREE_BUF_SZ];

	/* check DD bit on threshold descriptor */
	status = txq->ixgbe_tx_ring[txq->tx_next_dd].wb.status;
	if (!(status & IXGBE_ADVTXD_STAT_DD))
		return 0;

	n = txq->tx_rs_thresh;

	/*
	 * first buffer to free from S/W ring is at index
	 * tx_next_dd - (tx_rs_thresh-1)
	 */
	txep = &txq->sw_ring_vec[txq->tx_next_dd - (n - 1)];
	m = rte_pktmbuf_prefree_seg(txep[0].mbuf);
	if (likely(m != NULL)) {
		free[0] = m;
		nb_free = 1;
		for (i = 1; i < n; i++) {
			m = rte_pktmbuf_prefree_seg(txep[i].mbuf);
			if (likely(m != NULL)) {
				if (likely(m->pool == free[0]->pool))
					free[nb_free++] = m;
				else {
					rte_mempool_put_bulk(free[0]->pool,
							(void *)free, nb_free);
					free[0] = m;
					nb_free = 1;
				}
			}
		}
		rte_mempool_put_bulk(free[0]->pool, (void **)free, nb_free);
	} else {
		for (i = 1; i < n; i++) {
			m = rte_pktmbuf_prefree_seg(txep[i].mbuf);
			if (m != NULL)
				rte_mempool_put(m->pool, m);
		}
	}

	/* buffers were freed, update counters */
	txq->nb_tx_free = (uint16_t)(txq->nb_tx_free + txq->tx_rs_thresh);
	txq->tx_next_dd = (uint16_t)(txq->tx_next_dd + txq->tx_rs_thresh);
	if (txq->tx_next_dd >= txq->nb_tx_desc)
		txq->tx_next_dd = (uint16_t)(txq->tx_rs_thresh - 1);

	return txq->tx_rs_thresh;
}

static inline void
_ixgbe_rx_queue_release_mbufs_vec(struct ixgbe_rx_queue *rxq)
{
	unsigned int i;

	if (rxq->sw_ring == NULL || rxq->rxrearm_nb >= rxq->nb_rx_desc)
		return;

	/* free all mbufs that are valid in the ring */
	if (rxq->rxrearm_nb == 0) {
		for (i = 0; i < rxq->nb_rx_desc; i++) {
			if (rxq->sw_ring[i].mbuf != NULL)
				rte_pktmbuf_free_seg(rxq->sw_ring[i].mbuf);
		}
	} else {
		for (i = rxq->rx_tail;
		     i != rxq->rxrearm_start;
		     i = (i + 1) % rxq->nb_rx_desc) {
			if (rxq->sw_ring[i].mbuf != NULL)
				rte_pktmbuf_free_seg(rxq->sw_ring[i].mbuf);
		}
	}

	rxq->rxrearm_nb = rxq->nb_rx_desc;

	/* set all entries to NULL */
	memset(rxq->sw_ring, 0, sizeof(rxq->sw_ring[0]) * rxq->nb_rx_desc);
}

static inline void
_ixgbe_tx_free_swring_vec(struct ci_tx_queue *txq)
{
	if (txq == NULL)
		return;

	if (txq->sw_ring != NULL) {
		rte_free(txq->sw_ring_vec - 1);
		txq->sw_ring_vec = NULL;
	}
}

static inline void
_ixgbe_reset_tx_queue_vec(struct ci_tx_queue *txq)
{
	static const union ixgbe_adv_tx_desc zeroed_desc = { { 0 } };
	struct ci_tx_entry_vec *txe = txq->sw_ring_vec;
	uint16_t i;

	/* Zero out HW ring memory */
	for (i = 0; i < txq->nb_tx_desc; i++)
		txq->ixgbe_tx_ring[i] = zeroed_desc;

	/* Initialize SW ring entries */
	for (i = 0; i < txq->nb_tx_desc; i++) {
		volatile union ixgbe_adv_tx_desc *txd = &txq->ixgbe_tx_ring[i];

		txd->wb.status = IXGBE_TXD_STAT_DD;
		txe[i].mbuf = NULL;
	}

	txq->tx_next_dd = (uint16_t)(txq->tx_rs_thresh - 1);
	txq->tx_next_rs = (uint16_t)(txq->tx_rs_thresh - 1);

	txq->tx_tail = 0;
	txq->nb_tx_used = 0;
	/*
	 * Always allow 1 descriptor to be un-allocated to avoid
	 * a H/W race condition
	 */
	txq->last_desc_cleaned = (uint16_t)(txq->nb_tx_desc - 1);
	txq->nb_tx_free = (uint16_t)(txq->nb_tx_desc - 1);
	txq->ctx_curr = 0;
	memset(txq->ctx_cache, 0, IXGBE_CTX_NUM * sizeof(struct ixgbe_advctx_info));

	/* for PF, we do not need to initialize the context descriptor */
	if (!txq->is_vf)
		txq->vf_ctx_initialized = 1;
}

static inline int
ixgbe_txq_vec_setup_default(struct ci_tx_queue *txq,
			    const struct ixgbe_txq_ops *txq_ops)
{
	if (txq->sw_ring_vec == NULL)
		return -1;

	/* leave the first one for overflow */
	txq->sw_ring_vec = txq->sw_ring_vec + 1;
	txq->ops = txq_ops;
	txq->vector_tx = 1;

	return 0;
}

static inline int
ixgbe_rx_vec_dev_conf_condition_check_default(struct rte_eth_dev *dev)
{
#ifndef RTE_LIBRTE_IEEE1588
	struct rte_eth_fdir_conf *fconf = IXGBE_DEV_FDIR_CONF(dev);

	/* no fdir support */
	if (fconf->mode != RTE_FDIR_MODE_NONE)
		return -1;

	for (uint16_t i = 0; i < dev->data->nb_rx_queues; i++) {
		struct ixgbe_rx_queue *rxq = dev->data->rx_queues[i];
		if (!rxq)
			continue;
		if (!ci_rxq_vec_capable(rxq->nb_rx_desc, rxq->rx_free_thresh, rxq->offloads))
			return -1;
	}
	return 0;
#else
	RTE_SET_USED(dev);
	return -1;
#endif
}
#endif
