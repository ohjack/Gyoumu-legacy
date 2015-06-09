<?php
namespace Oerp\Purchase\Page;

class SupplierGoods extends \Oerp\Page\TreeView
{
  protected function getClassname() {
    return __CLASS__;
  }

  protected function hook_form_internal()  {
    $arch = file_get_contents(__DIR__ . '/SupplierGoods.xml');
    $param = array('arch' => $arch);

    if ($_GET['cri']) {
      $param['items_per_page'] = 800;
    }
    else {
      $param['criteria'][] = array('id', '=', 0);
    }

    $param['filter'] = '
<filter>
  <func>
    <src><![CDATA[
      return !!$rec[$name];
    ]]></src>
    <fld>product_id.func:sku</fld>
  </func>
</filter>
';
    $param['ex_ops'] =
        sprintf('<input class="ex-op" type="textfield" name="ex_ops-qty" value="%s" autocomplete="off"/>', 0);

    $this->args = array(0, $param);
    return parent::hook_form_internal();
  }

  public function hook_theme($form) {
    $src = parent::hook_theme($form);
    \Gulei\Util\Path::add_js(__DIR__ . '/SupplierGoods/SupplierGoods.js');
    \Gulei\Util\Path::add_css(__DIR__ . '/SupplierGoods/SupplierGoods.css');
    return $src;
  }

  protected function getToolbarBottom() {
    return $this->getLocalCalleeEle(array('Create_Order'));
  }
}