echo "Killing stale processes"
killall pid2d_main 2>/dev/null
killall command  2>/dev/null
killall aruco_test  2>/dev/null

echo "Go!"
cd /home/nvidia/BU-Autonomous-RaceCar/sensing/build/utils
./aruco_test live &>/dev/null &
cd /home/nvidia/BU-Autonomous-RaceCar/actuation
./command &>/dev/null &
cd /home/nvidia/BU-Autonomous-RaceCar/PID
./controller_main

