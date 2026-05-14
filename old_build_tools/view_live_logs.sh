#!/usr/bin/env bash

while true; do
  clear
  ssh mobile@192.168.0.133 "tail -n +1 -f /tmp/synthem.log"
  echo
  echo "Disconnected. Reconnecting in 2 seconds..."
  sleep 2
done
