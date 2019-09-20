#include <grub/dl.h>
#include <grub/err.h>
#include <grub/extcmd.h>
#include <grub/i18n.h>
#include <grub/misc.h>
#include <grub/mm.h>
#include <grub/types.h>

static grub_err_t grub_cmd_hello(struct grub_extcmd *cmd
				 __attribute__((unused)),
				 int argc __attribute__((unused)),
				 char **args __attribute__((unused))) {
  grub_printf("Hello World\n");
  return 0;
}

static grub_extcmd_t cmd;

GRUB_MOD_INIT(hello) {
  cmd = grub_register_extcmd("hello", grub_cmd_hello, GRUB_COMMAND_FLAG_BOTH, 0,
			     N_("Say \"Hello World\"."), 0);
}

GRUB_MOD_FINI(hello) { grub_unregister_extcmd(cmd); }
