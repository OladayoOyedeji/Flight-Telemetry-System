# Example QEMU launch command inside a Makefile
run:
	qemu-system-arm -machine lm3s6965evb -nographic -kernel build/telemetry_sys.bin
