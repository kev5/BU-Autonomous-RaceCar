echo "Go!"
cd /home/nvidia/BU-Autonomous-RaceCar/sensing/build/utils
./aruco_test live &>/dev/null &
cd /home/nvidia/BU-Autonomous-RaceCar/actuation
./command &>/dev/null &
cd /home/nvidia/BU-Autonomous-RaceCar/PID
./pid2d_main

