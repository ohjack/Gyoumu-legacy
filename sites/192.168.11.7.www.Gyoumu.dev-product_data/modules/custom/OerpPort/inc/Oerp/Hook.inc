<?php
namespace Oerp;

class Hook {
  static public function searchTreeHook($vname, $type) {
    $hooks = module_invoke_all('oerp_treeview_process');
    return $hooks[$vname][$type];
  }

  static public function searchFormHook($vname, $type) {
    $hooks = module_invoke_all('oerp_formview_process');
    return $hooks[$vname][$type];
  }

  static public function searchJsHook() {
    $hooks = module_invoke_all('oerp_js_hook');

    $func = create_function(
      '$a,$b', 'return $a->getWeight() > $b->getWeight();');

    usort($hooks, $func);
    return $hooks;
  }
}