#! /bin/bash
THIRD_PARTY_DIR="./samples/detection_analytics_plugin/lib"
DES_THIRD_PARTY_DIR="/opt/lib"
echo "Setup Environment to build Plugin on Linux"
read -sp "Pass Root:" zPassWord
#
# Make folder lib in /opt
[ -d "$DES_THIRD_PARTY_DIR/" ] && echo -e "\nFolder is exist!!!" || ((echo $zPassWord | sudo mkdir -p "$DES_THIRD_PARTY_DIR") && echo -e "\nCreate folder $DES_THIRD_PARTY_DIR!!!")
#
# Copy 3rd party to /opt/lib
echo "Copy library 3rd party to $DES_THIRD_PARTY_DIR"
echo $zPassWord | sudo cp -rp $THIRD_PARTY_DIR/*so $DES_THIRD_PARTY_DIR/
echo "Please double check before build your Plugin!!!"


