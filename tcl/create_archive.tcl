#-----------------------------------------------------------
# create_archive.tcl
#-----------------------------------------------------------
set design_name dma_ex_fft
set design_ver v1_0
set underscore _
puts "NOTE: This file must be executed from the project's 'tcl' directory"

#-----------------------------------------------------------
# Clean up extraneous files
#-----------------------------------------------------------
puts "Removing extraneous files..."
#exec rm "./vivado_*"
#exec rm -rf ".Xil"
#exec rm -rf "../proj/*"
#exec rm "../sim/blk_mem_gen_0.mif"
#exec rm "../sim/transcript"
#exec rm "../sim/vsim.wlf"
#exec rm -rf "../sim/msim"
#exec rm -rf "../sim/work"

#-----------------------------------------------------------
# Create archive
#-----------------------------------------------------------
puts "Creating archive..."
set format_date [exec date +%Y%m%d_%H%M]
set date_suffix $underscore$format_date

exec zip -r "../../$design_name$underscore$design_ver$date_suffix.zip" "../../$design_name$underscore$design_ver"

puts "Archive of $design_name$underscore$design_ver complete!"