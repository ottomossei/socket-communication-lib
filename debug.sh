#!/bin/bash
# UDS通信の状況を調査するスクリプト

# 特定のファイルやソケットを使用しているプロセスIDを調べる
echo "fuser /tmp/uds_socket"
fuser /tmp/uds_socket

# lsof（List Open Files）
# 特定のソケットを使用しているプロセスIDに加え、fd等の詳細な接続情報も確認
echo "lsof /tmp/uds_socket"
lsof /tmp/uds_socket

# ssコマンドでソケットの状態を確認
echo "ss -a | grep uds_socket"
ss -a | grep uds_socket
