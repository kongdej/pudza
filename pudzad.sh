#!/bin/bash
# /etc/init.d/pudzad

### BEGIN INIT INFO
# Provides:          pudza maker club
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example initscript
# Description:       This service is used to manage a servo
### END INIT INFO


case "$1" in 
    start)
        echo "Starting pudzad"
        /root/pudza/arduino.py > /dev/null 2>&1 &
        /root/pudza/reporter.py > /dev/null 2>&1 &

        ;;
    stop)
        echo "Stopping pudzad"
        killall python
        ;;
    *)
        echo "Usage: /etc/init.d/pudzad start|stop"
        exit 1
        ;;
esac

exit 0