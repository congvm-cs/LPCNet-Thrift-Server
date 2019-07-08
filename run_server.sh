export CURRENT_DIR=$PWD
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CURRENT_DIR/external_libraries/lame-3.100/cmake-build/libraries/lib/:$CURRENT_DIR/external_libraries/thrift/cmake-build/thrift-package/lib/:$CURRENT_DIR/external_libraries/fdk-aac/cmake-build/fdk-acc-package/lib/:/home/zdeploy/AILab/congvm/projects/LPCNet_CPP/LPCNet-Thrift-Server/external_libraries/libevent-2.1.8-stable/cmake-build/lib

#echo $LD_LIBRARY_PATH
./bin/run_lpcnet_server
