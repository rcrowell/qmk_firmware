make dz60:secretmods
echo "put board into RESET..."
sleep 2
sudo dfu-programmer atmega32u4 erase
sudo dfu-programmer atmega32u4 flash dz60_secretmods.hex 
sudo dfu-programmer atmega32u4 reset
