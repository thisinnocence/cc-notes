#!/bin/bash

# 查看下TIME_WATI，如果我们频繁多次运行这个脚本，server退出后仍然会有在time-wait状态的监听，不设置端口重用会监听失败
# TIME_WATI作用
#   确保数据完整性: TIME_WAIT 状态确保所有的 TCP 数据包在网络中完全被处理，避免因延迟的数据包与新连接产生冲突
#   防止重复连接: TIME_WAIT 状态有助于防止在旧连接的报文被重新接收时引发问题，确保新的连接不会因为旧的数据包而产生错误。
# 查看TIME_WAIT的值
sysctl net.netfilter.nf_conntrack_tcp_timeout_time_wait  ## 查看默认time-wait，Ubuntu 20是20s
ss -atn | grep 8080   # 查看被server监听的port，短时间会在time-wait

echo "[shell] Server starts in the background"
./server.out &
server_pid=$!  # 获取后台进程的 PID
sleep 0.1      # sleep 100ms, 方便server后台运行到了监听完毕的阶段

echo "[shell] Client starts in the foreground"
./client.out

echo "[shell] Waiting for the server to finish..."
wait $server_pid  # 等待后台进程结束

echo "======All programs are finished======"
