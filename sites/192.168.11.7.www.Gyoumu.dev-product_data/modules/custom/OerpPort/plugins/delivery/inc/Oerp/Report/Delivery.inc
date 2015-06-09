<?php
namespace Oerp\Report;

class Delivery extends \Gulei\Page\Form\_Abstract
{
  const ITEMS_PER_PAGE = 2500;
//  const DATE_FIELD = 'date';
//  const DATE_FIELD = 'date_planned';

  protected function getClassname()
  {
    return __CLASS__;
  }

  protected function getParam()
  {
    return null;
  }

  protected function getToolbarBottom()
  {
    return array();
  }

  private function getCri($year, $month, $type)
  {
    $Date = new \DateTime(sprintf('%s-%s-01', $year, $month));
    $start_date = $Date->format('Y-m-d');
    $Date->modify('+1 month');
    $Date->modify('-1 day');
    $end_date = $Date->format('Y-m-d');

    return array(
      array('location_dest_id', 'ilike', 'Customers'),
      array($type, '>=', $start_date),
      array($type, '<=', $end_date),
      array('state', 'ilike', 'done'),
    );
  }

  private function getProvParam()
  {
    $arch = <<<XML
<tree>
  <param name="oerp:model">stock.move</param>

  <field name="location_dest_id"/>
  <field name="product_id">
    <field name="default_code"/>
  </field>
  <field name="product_qty"/>
  <field name="picking_id">
    <field name="x_seller"/>
  </field>
  <field name="date"/>
  <field name="state"/>
</tree>
XML;

    list($year, $month, $type) = array($_GET['year'], $_GET['month'], $_GET['type']);
    $cri = $this->getCri($year, $month, $type);

    return array(
      'arch' => $arch,
      'model' => 'stock.move',
      'pager' => true,
      'items_per_page' => self::ITEMS_PER_PAGE,
      'oerp:criteria' => json_encode($cri),
      'criteria' => $cri,
    );
  }

  private function getProv($page)
  {
    $param = $this->getProvParam();
    $param['page'] = $page;
    return new \Oerp\Provider\TreeRecords($param);
  }

  private function getSrcRecs()
  {
    $prov = $this->getProv(0);
    $paramPager = \Gulei\Pager\Basic::getPagerParam(
      array(
        'items_per_page' => self::ITEMS_PER_PAGE,
        'count' => $prov->count()
      )
    );

    $recs = array();
    for($page = 0; $page < $paramPager->total; $page++){
       $prov = $this->getProv($page);
      foreach($prov->getData() as $rec){
         $recs[] = $rec;
      }
    }

    return $recs;
  }

  protected function getTableObj()
  {
    _erp_connect(null, 'maolung', 'admin', '2xaugadr');

    $recs = array();
    $sellers = array();

    foreach($this->getSrcRecs() as $each){
      if($each['id']){
        list($sku, $seller, $qty) = array(
          $each['product_id.default_code'],
          $each['picking_id.x_seller'],
          $each['product_qty'],
        );

        if(!in_array($seller, $sellers, true)){
          $sellers[] = $seller;
        }

        if(!isset($recs[$sku]['_sum'])){
          $recs[$sku]['_sum'] = $qty;
        }
        else{
          $recs[$sku]['_sum'] += $qty;
        }

        if(!isset($recs[$sku][$seller])){
          $recs[$sku][$seller] = $qty;
        }
        else{
          $recs[$sku][$seller] += $qty;
        }
      }
    }

    foreach($recs as $sku => &$rec){
      $rec['_sku'] = $sku;
      foreach($sellers as $seller){
        if(!isset($rec[$seller])){
          $rec[$seller] = 0;
        }
      }
    }

    ksort($recs);
    sort($sellers);

    $hds = array_merge(array('_sku', '_sum'), $sellers);
    $hds = array_combine($hds, $hds);

    return new \Gulei\Table\Simple($hds, $recs);
  }

  protected function hook_form_internal()
  {
    $Tb = $this->getTableObj();

    $form = array();
    $form['tree'] = array(
      '#type' => 'markup',
      '#value' => $Tb->theme(),
    );

    $form['csv'] = array(
      '#type' => 'textarea',
      '#value' => $Tb->dumpCsv(),
    );

    return $form;
  }
}