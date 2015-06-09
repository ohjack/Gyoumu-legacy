<?php
namespace Oerp;
use Gulei as G;

class ProviderProductInDevelopmentRecords implements G\InterfaceProvider
{
  public function getReference()
  {
    return array();
  }

  private function getArchOrder($id)
  {
    return <<<XML
<tree>
  <param name="oerp:model">purchase.order.line</param>
  <param name="oerp:criteria">[["product_id", "=", {$id}]]</param>

  <field name="order_id">
    <field name="name" string="Ref"/>
    <field name="state" string="Status"/>
    <field name="date_order" string="Created"/>
    <field name="minimum_planned_date" string="Planned"/>
    <field name="partner_id" string="Supplier"/>
  </field>
</tree>
XML;
  }

  private function getOrder($id)
  {
    $q = new \OerpNestQuery($this->getArchOrder($id));
    $send = array();

    foreach($q->sum() as $order){
      $send[] = $order['order_id'];
    }
    return $send;
  }

  private function getOrderFields()
  {
    $Ele = new \OerpArchElement($this->getArchOrder(0));
    return array_keys($Ele->extractAttr('name'));
  }

  private function getArchProduct()
  {
    return <<<XML
<tree>
  <param name="oerp:model">product.product</param>
  <param name="oerp:criteria">[["state", "=", "draft"]]</param>

  <field name="id"/>
  <field name="default_code"/>
  <field name="name"/>
  <field name="qty_available"/>
  <field name="seller_ids">
    <field name="name"/>
  </field>
</tree>
XML;
  }

  private function getProductFields()
  {
    $Ele = new \OerpArchElement($this->getArchProduct());
    return array_keys($Ele->extractAttr('name'));
  }

  private function getProducts()
  {
    $q = new \OerpNestQuery($this->getArchProduct());
    $recs = $q->sum();

    foreach($recs as &$rec){
      $supps = array();
      foreach($rec['seller_ids'] as $supp){
        $supps[] = $supp['name'];
      }
      $rec['seller_ids'] = implode($supps, '<br/>');
    }
    return $recs;
  }

  private function getAllFields()
  {
    return array_merge($this->getProductFields(), $this->getOrderFields());
  }

  public function getData()
  {
    $recs = $this->getProducts();

    foreach($recs as &$rec){
      $rec['order_id'] = (array)$this->getOrder($rec['id']);
    }

    $Arry = new \OerpArray($recs);
    $data = $Arry->supplement($this->getAllFields(), 'ditto');

    foreach($data as &$datum){
      if(!isset($datum['order_id.name'])){
        $datum['order_id.name'] = 'N/A';
      }
    }
    return $data;
  }

  public function count()
  {
    $q = new \OerpQuery('product.product', array('state' => 'draft'), array('id'));
    return count($q->raw());
  }
}

class ProductInDevelopmentPageController extends \FormPageController
{
  public function hook_form(&$form_state)
  {
    $arch = <<<XML
  <tree>
    <!--<param name="oerp:model">product.product</param>-->
    <param name="oerp:op-type">XUX</param>

    <field name="default_code" string="SKU"/>
    <field name="name" string="Name"/>
    <field name="qty_available" string="Stock Level"/>
    <field name="order_id.name" string="Order ID"/>
    <field name="order_id.state" string="Status"/>
    <field name="order_id.minimum_planned_date" string="Planned"/>
  </tree>
XML;

    $t = new \GuArchTable($arch, new ProviderProductInDevelopmentRecords());
    $t->mods['hd']['raw'] = array(new \OerpPackHeaderRawMods());
    $t->mods['row']['raw'] = array(new \OerpPackRowRawMods());

    $form = array();
    $form['view'] = array(
      '#type' => 'markup',
      '#value' => $t->theme(
        null, null,
        array(
             'attrs' => array(
               'class' => 'oerp-treeview',
               'oerp_model' => 'product.product',
             )
        )
      ),
      '#prefix' => '<div class="caption">Products in Development</div>'
    );

    \IncluderOerpView::includeCss();
    \IncluderOerpView::includeJs();
    return $form;
  }
}