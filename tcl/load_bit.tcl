#-----------------------------------------------------------
# load_bit.tcl
#-----------------------------------------------------------
puts "NOTE: This file must be executed from the project's 'tcl' directory"

#-----------------------------------------------------------
# Load the bitstream into the PL
#-----------------------------------------------------------
puts "Loading bitstream into the PL..."

xconnect arm hw  -debugdevice cpunr 1 devicenr 1
set_cur_target 64
set_cur_system
reset_zynqpl
xdisconnect 64
xdisconnect 352
set_cur_system_target
xfpga -f "../bin/system.bit" -debugdevice deviceNr 2 -report_progress

# NOTE: This probably fails for 32-bit machines
