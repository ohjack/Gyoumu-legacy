<?
namespace Oerp\Stock\Page;

class RecentDelivered extends \Gulei\Page\Form\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  protected function hook_form_internal() {
    $form = array();

    $provMeta = new \Oerp\Product\Provider\Meta();
    $param = $provMeta->getParam();

    $remote_recs = \Oerp\Query::q(
      'product.product', $param['oerp:criteria'], array('id', 'default_code'));

    $ids = $remote_recs->collect('id');
    $remote_recs = $remote_recs->promote('product_tmpl_id');

    $provDelivered = new \Oerp\Stock\Provider\RecentDelivered($ids);
    $recs = $provDelivered->getData();

    foreach ($recs as $id => &$rec) {
      $rec['sku'] = $remote_recs[$rec['id']]['default_code'];
    }

    $arch = <<<XML
<tree>
  <field name="sku" string="sku"/>
  <field name="qty_available" string="sent"/>
</tree>
XML;

    $Table = new \Gulei\Table\Arch($arch, new \Gulei\Provider\Lambda($recs));

    $form['src'] = array(
      '#type' => 'markup',
      '#value' => $Table->theme(),
    );
    return $form;
  }

  protected function getToolbarBottom() {
    return array();
  }

  public function getParam() {
    return array();
  }
}