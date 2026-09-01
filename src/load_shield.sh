#!/bin/bash
set -e

INTERFACE="enp0s31f6"

echo "[*] L0: Wstrzykiwanie Stealth UDP Auth (XDP)..."
ip link set dev $INTERFACE xdp off 2>/dev/null || true
rm -f /sys/fs/bpf/authorized_nodes
ip link set dev $INTERFACE xdp obj /opt/barkan/xdp_barkan_v3.o sec xdp
bpftool map pin id $(bpftool map show | grep authorized_node | tail -1 | cut -d: -f1) /sys/fs/bpf/authorized_nodes

echo "[*] L4: Wstrzykiwanie TCP Kernel Shield (Sysctl)..."
sysctl -p /etc/sysctl.d/99-barkan-tuning.conf

echo "[*] L7: Wstrzykiwanie Slowloris Guillotine (SockOps)..."
rm -f /sys/fs/bpf/barkan_sockops
bpftool prog load /opt/barkan/sockops_barkan.o /sys/fs/bpf/barkan_sockops
bpftool cgroup attach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/barkan_sockops

echo "[*] MATRYCA BARKAN V3 W PEŁNI AKTYWNA."
