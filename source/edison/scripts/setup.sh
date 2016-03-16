echo "Starting Install Script..."

echo "src all http://iotdk.intel.com/repos/2.0/iotdk/all" > /etc/opkg/base-feeds.conf
echo "src x86 http://iotdk.intel.com/repos/2.0/iotdk/x86" >> /etc/opkg/base-feeds.conf
echo "src i586 http://iotdk.intel.com/repos/2.0/iotdk/i586" >> /etc/opkg/base-feeds.conf

opkg update
opkg install git

git clone https://github.com/CPSSD/robots.git

cd $HOME
mkdir GoInstallation
cd GoInstallation

wget https://storage.googleapis.com/golang/go1.6.linux-386.tar.gz
tar -zxvf go1.6.linux-386.tar.gz
mv go /usr/local/

cd ..
rm -r GoInstallation

echo "export PATH=$PATH:/usr/local/go/bin" > $HOME/.profile
echo "export GOPATH=:$HOME/robots/source/edison" >> $HOME/.profile
echo "export GOBIN=:$GOPATH/bin" >> $HOME/.profile
source .profile
