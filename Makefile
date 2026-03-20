all: 
	echo "Nothing is run"

erase_ca80_old:
	pyocd erase --sector 0x8000000+20480 -t pic32cz8110ca80208  --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA80_DFP.1.7.199.pack"

flash_ca80_old: erase_ca80_old
	pyocd flash -e sector -a 0x8000000 -t pic32cz8110ca80208 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex' --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA80_DFP.1.7.199.pack"

erase_ca80:
	pyocd erase --sector 0x8000000+20480 -t pic32cz8110ca80208  --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA80_DFP.1.7.202.pack" -O adi.v5.max_invalid_ap_count=0

flash_ca80: erase_ca80
	pyocd flash -e sector -a 0x8000000 -t pic32cz8110ca80208 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex' --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA80_DFP.1.7.202.pack" -O adi.v5.max_invalid_ap_count=0

erase_ca90:
	pyocd erase --sector 0x8000000+20480 -t pic32cz8110ca90208 --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA90_DFP.1.8.199.pack"

flash_ca90: erase_ca90
	pyocd flash -e sector -a 0x8000000 -t pic32cz8110ca90208 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex' --pack "C:\Users\x55885\Downloads\Microchip.PIC32CZ-CA90_DFP.1.8.199.pack"

flash_sg61: 
	pyocd flash -e sector -t pic32cx1025sg61128 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex' -O adi.v5.max_invalid_ap_count=0

flash_sg41: 
	pyocd flash -e sector -t pic32cx1025sg41128 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex' -O adi.v5.max_invalid_ap_count=0

erase_same54:
	 pyocd erase --sector 0x0 -t atsame54p20a

flash_same54:
	pyocd flash -e sector -t atsame54p20a 'C:\developers\zephyrproject\zephyr\build\zephyr\zephyr.hex'

cmsis_switch:
	pycmsisdapswitcher --target evalboard --source=server --fwtype=cmsis

pkob_switch:
	pycmsisdapswitcher --target evalboard --source=server --fwtype=mplab
