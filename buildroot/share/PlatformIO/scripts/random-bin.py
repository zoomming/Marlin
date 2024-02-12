#
# random-bin.py
# Set a unique firmware name based on current date and time
#
import pioutil
if pioutil.is_pio_build():
    from datetime import datetime
    Import("env")
    #env['PROGNAME'] = datetime.now().strftime("firmware-%Y%m%d-%H%M%S")
    GDCpu = env['PIOENV']
    if (GDCpu == 'GD32F303RE_creality') or (GDCpu == 'GD32F303RE_creality'):
        env['PROGNAME'] = datetime.now().strftime("GD-Ender-3 V2-firmware-%Y%m%d-%H%M%S")
    else:
        env['PROGNAME'] = datetime.now().strftime("firmware-%Y%m%d-%H%M%S")
