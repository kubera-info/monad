##########################################################################
### Copyright © Wlodzimierz O. Kubera. Licensed under the MIT License. ###
##########################################################################

MND_RAW_BIN_DIR=..\..\..\..\MonadEngine\bin
MND_BIN_DIR=@$(MND_RAW_BIN_DIR)
MND_ENCRYPT=$(MND_BIN_DIR)\Encryption.exe $(MND_SALT) $? $@

Init:
	@echo the Kubera's private solution
	@echo $(MND_TITLE)
	@echo Copyright © 2016-2026 by Wlodzimierz O. Kubera
	@echo All rights reserved