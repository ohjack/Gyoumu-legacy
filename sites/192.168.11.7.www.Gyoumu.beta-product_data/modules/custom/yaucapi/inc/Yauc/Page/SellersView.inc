<?php
namespace Yauc\Page;

class SellersView extends \Gulei\Page\Form\Basic
{
  protected function getActionLinks($seller)
  {
    $links = array();
    $links['auction'] =  array(
      l('Active', 'yaucapi/seller/' . $seller->seller_id . '/auctions/view/active'),
      l('Sold', 'yaucapi/seller/' . $seller->seller_id . '/auctions/view/sold'),
      l('Unsold', 'yaucapi/seller/' . $seller->seller_id . '/auctions/view/unsold'),
      l('Relist', 'yaucapi/seller/' . $seller->seller_id . '/auctions/view/relist')
    );
    return $links;
  }

  protected function getActionSrc($seller)
  {
    $links = $this->getActionLinks($seller);
    foreach($links as &$group){
      $group = implode(' | ', $group);
    }
    return '<div>' . implode('<br/>', $links) . '</div>';
  }

  protected function getProvider()
  {
    $sellers = _yaucapi_get_sellers();

    $recs = array();
    foreach($sellers as $seller){
      $rec = array();
      $rec['sel_id'] = $seller->seller_id;
      $rec['actions'] = $this->getActionSrc($seller);

      $y = new \GuYapi($seller->seller_id);
      $rec['msg'] = $y->getTokenCheckMessage();
      $recs[] = $rec;
    }

    return new \Gulei\Provider\Lambda($recs);
  }

  protected function getTableObj()
  {
    $arch = <<<XML
  <arch>
    <field name="sel_id" string="Account"/>
    <field name="actions" string="Actions"/>
    <field name="msg" string=" "/>
  </arch>
XML;

    return new \Gulei\Table\Arch($arch, $this->getProvider());
  }

  public function hook_form(&$form_state)
  {
    return array(
      'view' => array(
        '#type' => 'markup',
        '#value' => $this->getTableObj()->theme(),
      ),
    );
  }
}