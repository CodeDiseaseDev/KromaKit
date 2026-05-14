sleep 1

ssh mobile@192.168.0.133 \
  "pid=\$(ps aux | grep '[S]ynthem' | awk '{print \$2}' | head -n 1) && debugserver \"127.0.0.1:1234\" --attach=\$pid"

sleep 1

ssh -f -N -L 1234:127.0.0.1:1234 mobile@192.168.0.133

sleep 1

lldb ./Payload/Synthem.app/Synthem \
  -o "gdb-remote 127.0.0.1:1234"
