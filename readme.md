## Install
```
sudo apt update
sudo apt upgrade -y
```
\
**Fish:**
```
sudo apt install fish -y
chsh -s /usr/bin/fish
```
\
**Dropbox:**
```
sudo apt install rclone
rclone config
// n  dropbox  Client ID and Secret empty  default settings
rclone ls dropbox:
sudo apt install fuse
mkdir ~/dropbox
rclone mount dropbox:  ~/dropbox ?daemon
```
\
**Qt:**
```
sudo apt install clang
sudo apt install qt5-qmake qtbase5-dev qtchooser qt5-qmake-bin qtbase5-dev-tools qtdeclarative5-dev
sudo apt install qtcreator
qtcreator
```
\
**Visual Studio Code:**
```
sudo apt install software-properties-common apt-transport-https wget -y
wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | sudo gpg --dearmor -o /usr/share/keyrings/microsoft-archive-keyring.gpg
echo "deb [arch=arm64 signed-by=/usr/share/keyrings/microsoft-archive-keyring.gpg] https://packages.microsoft.com/repos/code stable main" | sudo tee /etc/apt/sources.list.d/vscode.list
sudo apt install code -y
code
```
\
**LibreOffice + UNO-API:**
```
sudo apt install libreoffice -y
sudo apt install python3-uno -y
libreoffice
python3 -c ?import uno print(?UNO API works!?)?
```
\
**Shared Network Folder:**
```
sudo apt install samba
mkdir -p ~/SharedFolder
sudo nano /etc/samba/smb.cong -> Add to end:

[SharedFolder]
path = /home/pi/SharedFolder
browseable = yes
read only = no
guest ok = yes
sudo systemctl restart smbd
sudo smbpasswd -a admin (Add password for user admin)
smb:://raspberrypi.local/SharedFolder (Open in Finder -> CMD + K)
```
\
**Copy Folder:**
```
scp  -r  folderName  admin@raspberrypi:/home/admin/Desktop
```
\
**LibreOffice-Server:**
```
libreoffice --headless --invisible --accept="socket,host=localhost,port=2002;urp;StarOffice.ComponentContext" --calc
ps aux | grep soffice
kill <PID>
```
\
**Telnet:**
```
sudo apt install telnet
telnet 127.0.0.1 2002 // Check for libreOffice-Server
```

**Autostart + Desktop Entry:**
```
nano ~/Desktop/ZeitsystemStarten@.desktop
[Desktop Entry]
Type=Application
Name=ZeitsystemStarten
Exec=/home/admin/Desktop/build-TimeStampTracker-Desktop-Debug/TimeStampTracker
Terminal=true

chmod +x ~/Desktop/ZeitsystemStarten@.desktop

mkdir -p ~/.config/autostart

cp ~/Desktop/ZeitsystemStarten@.desktop ~/.config/autostart/

chmod +x ~/.config/autostart/ZeitsystemStarten@.desktop

```
