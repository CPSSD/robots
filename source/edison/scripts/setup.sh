echo "Starting Install Script..."

# Adds the package lists to opkg
echo "src all http://iotdk.intel.com/repos/2.0/iotdk/all" > /etc/opkg/base-feeds.conf
echo "src x86 http://iotdk.intel.com/repos/2.0/iotdk/x86" >> /etc/opkg/base-feeds.conf
echo "src i586 http://iotdk.intel.com/repos/2.0/iotdk/i586" >> /etc/opkg/base-feeds.conf

# Updates the package list and installs Go.
opkg update
opkg install git

# Clones the repo
git clone https://github.com/CPSSD/robots.git

# Creates a directory to download Go
cd $HOME
mkdir GoInstallation
cd GoInstallation

# Downloads Go and moves it to the correct location.
wget https://storage.googleapis.com/golang/go1.6.linux-386.tar.gz
tar -zxvf go1.6.linux-386.tar.gz
mv go /usr/local/

# Deletes the files used to install Go.
cd ..
rm -r GoInstallation

# Adds Go to the Path and creates the GOPATH and GOBIN variables.
echo "export PATH=$PATH:/usr/local/go/bin" > $HOME/.profile
echo "export GOPATH=$HOME/robots/source/edison" >> $HOME/.profile
echo "export GOBIN=$GOPATH/bin" >> $HOME/.profile
source .profile
