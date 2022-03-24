/*
 * Copyright (c) 2021 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _RMNET_QMI_I_H
#define _RMNET_QMI_I_H

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/timer.h>
#include <uapi/linux/rtnetlink.h>
#include <linux/soc/qcom/qmi.h>

#define MAX_MQ_NUM 16
#define MAX_CLIENT_NUM 2
#define MAX_FLOW_NUM 32
#define DEFAULT_GRANT 1
#define DEFAULT_CALL_GRANT 20480
#define DFC_MAX_BEARERS_V01 16
#define DEFAULT_MQ_NUM 0
#define ACK_MQ_OFFSET (MAX_MQ_NUM - 1)
#define INVALID_MQ 0xFF

#define DFC_MODE_SA 4

#define CONFIG_QTI_QMI_RMNET 1
#define CONFIG_QTI_QMI_DFC  1
#define CONFIG_QTI_QMI_POWER_COLLAPSE 1

extern int dfc_mode;
extern int dfc_qmap;

struct qos_info;

enum {
	RMNET_CH_DEFAULT,
	RMNET_CH_LL,
	RMNET_CH_MAX,
	RMNET_CH_CTL = 0xFF
};

enum rmnet_ch_switch_state {
	CH_SWITCH_NONE,
	CH_SWITCH_STARTED,
	CH_SWITCH_ACKED,
	CH_SWITCH_FAILED_RETRY
};

struct rmnet_ch_switch {
	u8 current_ch;
	u8 switch_to_ch;
	u8 retry_left;
	u8 status_code;
	enum rmnet_ch_switch_state state;
	__be32 switch_txid;
	u32 flags;
	bool timer_quit;
	struct timer_list guard_timer;
	u32 nl_pid;
	u32 nl_seq;
};

struct rmnet_bearer_map {
	struct list_head list;
	u8 bearer_id;
	int flow_ref;
	u32 grant_size;
	u32 grant_thresh;
	u16 seq;
	u8  ack_req;
	u32 last_grant;
	u16 last_seq;
	u32 bytes_in_flight;
	u32 last_adjusted_grant;
	bool tcp_bidir;
	bool rat_switch;
	bool tx_off;
	u32 ack_txid;
	u32 mq_idx;
	u32 ack_mq_idx;
	struct qos_info *qos;
	struct timer_list watchdog;
	bool watchdog_started;
	bool watchdog_quit;
	u32 watchdog_expire_cnt;
	struct rmnet_ch_switch ch_switch;
};

struct rmnet_flow_map {
	struct list_head list;
	u8 bearer_id;
	u32 flow_id;
	int ip_type;
	u32 mq_idx;
	struct rmnet_bearer_map *bearer;
};

struct svc_info {
	u32 instance;
	u32 ep_type;
	u32 iface_id;
};

struct mq_map {
	struct rmnet_bearer_map *bearer;
	bool is_ll_ch;
};

struct qos_info {
	struct list_head list;
	u8 mux_id;
	struct net_device *real_dev;
	struct net_device *vnd_dev;
	struct list_head flow_head;
	struct list_head bearer_head;
	struct mq_map mq[MAX_MQ_NUM];
	u32 tran_num;
	spinlock_t qos_lock;
	struct rmnet_bearer_map *removed_bearer;
};

struct qmi_info {
	int flag;
	void *wda_client;
	void *wda_pending;
	void *dfc_clients[MAX_CLIENT_NUM];
	void *dfc_pending[MAX_CLIENT_NUM];
	bool dfc_client_exiting[MAX_CLIENT_NUM];
	unsigned long ps_work_active;
	bool ps_enabled;
	bool dl_msg_active;
	bool ps_ignore_grant;
	int ps_ext;
};

enum data_ep_type_enum_v01 {
	DATA_EP_TYPE_ENUM_MIN_ENUM_VAL_V01 = INT_MIN,
	DATA_EP_TYPE_RESERVED_V01 = 0x00,
	DATA_EP_TYPE_HSIC_V01 = 0x01,
	DATA_EP_TYPE_HSUSB_V01 = 0x02,
	DATA_EP_TYPE_PCIE_V01 = 0x03,
	DATA_EP_TYPE_EMBEDDED_V01 = 0x04,
	DATA_EP_TYPE_ENUM_MAX_ENUM_VAL_V01 = INT_MAX
};

struct data_ep_id_type_v01 {

	enum data_ep_type_enum_v01 ep_type;
	u32 iface_id;
};

extern struct qmi_elem_info data_ep_id_type_v01_ei[];

void *qmi_rmnet_has_dfc_client(struct qmi_info *qmi);

#ifdef CONFIG_QTI_QMI_DFC
struct rmnet_flow_map *
qmi_rmnet_get_flow_map(struct qos_info *qos_info,
		       u32 flow_id, int ip_type);

struct rmnet_bearer_map *
qmi_rmnet_get_bearer_map(struct qos_info *qos_info, u8 bearer_id);

unsigned int qmi_rmnet_grant_per(unsigned int grant);

int dfc_qmi_client_init(void *port, int index, struct svc_info *psvc,
			struct qmi_info *qmi);

void dfc_qmi_client_exit(void *dfc_data);

void dfc_qmi_burst_check(struct net_device *dev, struct qos_info *qos,
			 int ip_type, u32 mark, unsigned int len);

int qmi_rmnet_flow_control(struct net_device *dev, u32 mq_idx, int enable);

void dfc_qmi_query_flow(void *dfc_data);

int dfc_bearer_flow_ctl(struct net_device *dev,
			struct rmnet_bearer_map *bearer,
			struct qos_info *qos);

int dfc_qmap_client_init(void *port, int index, struct svc_info *psvc,
			 struct qmi_info *qmi);

void dfc_qmap_client_exit(void *dfc_data);

void dfc_qmap_send_ack(struct qos_info *qos, u8 bearer_id, u16 seq, u8 type);

struct rmnet_bearer_map *qmi_rmnet_get_bearer_noref(struct qos_info *qos_info,
						    u8 bearer_id);

void qmi_rmnet_watchdog_add(struct rmnet_bearer_map *bearer);

void qmi_rmnet_watchdog_remove(struct rmnet_bearer_map *bearer);

int rmnet_ll_switch(struct net_device *dev, struct tcmsg *tcm, int attrlen);
void rmnet_ll_guard_fn(struct timer_list *t);
void rmnet_ll_wq_init(void);
void rmnet_ll_wq_exit(void);
#else
static inline struct rmnet_flow_map *
qmi_rmnet_get_flow_map(struct qos_info *qos_info,
		       uint32_t flow_id, int ip_type)
{
	return NULL;
}

static inline struct rmnet_bearer_map *
qmi_rmnet_get_bearer_map(struct qos_info *qos_info, u8 bearer_id)
{
	return NULL;
}

static inline int
dfc_qmi_client_init(void *port, int index, struct svc_info *psvc,
		    struct qmi_info *qmi)
{
	return -EINVAL;
}

static inline void dfc_qmi_client_exit(void *dfc_data)
{
}

static inline int
dfc_bearer_flow_ctl(struct net_device *dev,
		    struct rmnet_bearer_map *bearer,
		    struct qos_info *qos)
{
	return 0;
}

static inline int
dfc_qmap_client_init(void *port, int index, struct svc_info *psvc,
		     struct qmi_info *qmi)
{
	return -EINVAL;
}

static inline void dfc_qmap_client_exit(void *dfc_data)
{
}

static inline void qmi_rmnet_watchdog_remove(struct rmnet_bearer_map *bearer)
{
}

static int rmnet_ll_switch(struct net_device *dev,
			   struct tcmsg *tcm, int attrlen)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_QTI_QMI_POWER_COLLAPSE
int
wda_qmi_client_init(void *port, struct svc_info *psvc, struct qmi_info *qmi);
void wda_qmi_client_exit(void *wda_data);
int wda_set_powersave_mode(void *wda_data, u8 enable);
void qmi_rmnet_flush_ps_wq(void);
void wda_qmi_client_release(void *wda_data);
int dfc_qmap_set_powersave(u8 enable, u8 num_bearers, u8 *bearer_id);
#else
static inline int
wda_qmi_client_init(void *port, struct svc_info *psvc, struct qmi_info *qmi)
{
	return -EINVAL;
}

static inline void wda_qmi_client_exit(void *wda_data)
{
}

static inline int wda_set_powersave_mode(void *wda_data, u8 enable)
{
	return -EINVAL;
}
static inline void qmi_rmnet_flush_ps_wq(void)
{
}
static inline void wda_qmi_client_release(void *wda_data)
{
}
#endif
#endif /*_RMNET_QMI_I_H*/
