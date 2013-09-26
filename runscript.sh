#!/bin/bash

SERVERBIN="sqserver"
PIDFILE="${SERVERBIN}.pid"
LOGDIR="$(pwd)/logs"
COMMANDLINE_PARAMETERS="-s ${LOGDIR}"

start() {
  PID=$(./${SERVERBIN} ${COMMANDLINE_PARAMETERS})
  echo -n $PID > $PIDFILE
  echo "server started with pid: $PID"
}

stop() {
  echo -n "stopping server..."
  if ( kill -TERM $(cat $PIDFILE) 2> /dev/null ); then
    i=1
    while [ "$i" -le 60 ]; do
      if (kill -0 $(cat $PIDFILE) 2> /dev/null )
        then
          echo -n "."
          sleep 1
        else
          break
      fi
      i=$((++i))
    done
  fi
  echo ""
  if (kill -0 $(cat $PIDFILE) 2> /dev/null);
    then
      echo -e "server is not shutting down correctly...\nkilling server"
      kill -KILL $(cat $PIDFILE) 2> /dev/null
    else
      echo "done"
  fi
  rm -f $PIDFILE
}

if [ ! -e $SERVERBIN ]; then
  echo "couldn't find executable"
  exit 1
fi

if [ ! -x $SERVERBIN ]; then
  echo "${SERVERBIN} is not executable, trying to set it"
  chmod +x $SERVERBIN
  if [ ! -x $SERVERBIN ]; then
    echo "couldn't start the server"
    exit 2
  fi
fi

if [ ! -d $LOGDIR ]; then
  echo "the directory $LOGDIR don't exist, creating..."
  mkdir $LOGDIR
  if [ ! -d $LOGDIR ]; then
    echo "couldn't create $LOGDIR"
    exit 3
  fi
fi
      
case "$1" in
  start)    if [ -e $PIDFILE ]
              then 
                if (kill -0 $(cat $PIDFILE) 2> /dev/null );
                  then
                    echo "server is already running"
                  else
                    rm -f $PIDFILE
                    start
                fi
              else
                start
            fi
            ;;
  stop)     if [ -e $PIDFILE ]
              then
                if (kill -0 $(cat $PIDFILE) 2> /dev/null );
                  then
                    stop
                  else
                    echo "server is not running"
                    rm -f $PIDFILE
                fi
              else
                echo "server is not running"
            fi
            ;;
  restart)  $0 stop && $0 start
            ;;
  *)        if [ -e $PIDFILE ]
              then
                if (kill -0 $(cat $PIDFILE) 2> /dev/null );
                  then
                    echo "server is running"
                  else
                    echo "server is not running"
                    rm -f $PIDFILE
                fi
              else
                echo "---"
            fi
            ;;
esac

