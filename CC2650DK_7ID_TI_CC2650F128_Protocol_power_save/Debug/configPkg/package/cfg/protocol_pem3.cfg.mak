# invoke SourceDir generated makefile for protocol.pem3
protocol.pem3: .libraries,protocol.pem3
.libraries,protocol.pem3: package/cfg/protocol_pem3.xdl
	$(MAKE) -f C:\Users\03468\workspace_v7\CC2650DK_7ID_TI_CC2650F128_Protocol_power_save/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\03468\workspace_v7\CC2650DK_7ID_TI_CC2650F128_Protocol_power_save/src/makefile.libs clean

