<?php
namespace Oerp\Delivery\Page\CargoReceipt;

class SPack extends \Gulei\Page\_Abstract
{
  private $carrs;
  private $order_recs;

  public function __construct($args)
  {
    parent::__construct($args);
    $this->init();
  }

  private function init()
  {
    $this->carrs = explode('|', $this->args[0]);
    $this->order_recs = $this->getOrderRecs();
  }

  private function getOrderRecs()
  {
    $q = new \Oerp\Query\Basic('delivery.carrier', array(array('name', 'in', $this->carrs)));
    $carrs_ids = $q->raw()->collect('id');

    $Date = new \DateTime();
    $Date->modify('-1 day');
    $close_time = $Date->format('Y-m-d 23:59');

    $param = array(
      'no-blank-tail' => true,
      'oerp:cri_ex' => array(
        array('carrier_id', 'in', $carrs_ids),
        array('date', '<=', $close_time)
      )
    );

    $prov = new \Oerp\Delivery\Provider\Tree\SPackCR($param);
    $orders = $prov->getData();
    $pids = \Gulei\_Array::lambda($orders, 'collect', 'move_lines.product_id.id');

    $q = new \Oerp\Query\Basic(
      'product.packaging', array(array('product_id', 'in', $pids)), array('product_id', 'weight', 'ul'));

    $pkg_src = $q->raw();
    $pkgs = array();

    foreach($pkg_src as $pkg){
      $pkg_name = $pkg['ul'][1];

      if(stripos($pkg_name, 'fedex') !== 0){
        preg_match('/\[(.*)\]/', $pkg['product_id'][1], $m);
        $pkgs[$m[1]] = array(
          'name' => $pkg_name,
          'wt' => $pkg['weight']
        );
      }
    }

    $order_recs = array();
    $count = 1;

    foreach($orders as $order){
      $sku = $order['move_lines.product_id.default_code'];
      $qty = $order['move_lines.product_qty'];

      if($order['name']){
        $newOrder = array(
          'count' => $count++,
          'packid' => $order['name'],
          'tnum' => $order['x_tnum'],
          'qty' => '1',
          'buyer' => $order['address_id.name'],
          'country' => $order['address_id.state_id.country_id'],
        );

        if($wt = $pkgs[$sku]['wt']){
          $newOrder['wt'] = $wt * $qty;
        }
        else {
          $newOrder['wt'] = 'N/A';
        }
        $order_recs[] = $newOrder;
      }
      else{
        $lastOrder = &$order_recs[count($order_recs) - 1];

        if(is_numeric($lastOrder['wt'])){
          if($wt = $pkgs[$sku]['wt']){
            $lastOrder['wt'] += $qty * $wt;
          }
          else{
            $lastOrder['wt'] = 'N/A';
          }
        }

        unset($lastOrder);
      }
    }
    return $order_recs;
  }

  private function themeList()
  {
    $hds = array(
      'count' => '#',
      'packid' => 'Ref.',
      'tnum' => 'Track #',
      'qty' => 'Qty',
      'buyer' => 'Buyer',
      'country' => 'Country',
      'wt' => 'Weight(g)',
    );

    $buf = '';
    $buf .= '<h2>' . implode(',', $this->carrs) . '</h2>';

    $tb = new \Gulei\Table\Simple($hds, $this->order_recs);
    $buf .=  $tb->theme(null, null, array('attrs' => array('class' => 'print')));
    return $buf;
  }

  private function getSummaryHds()
  {
    $hds = array();
    $hds['cap'] = '級距';
    $hds['na'] = 'N/A';

    for($c = 20; $c <= 800; $c += 20){
      $hds[$c] = $c;
    }
    return$hds;
  }

  private function getSummaryTable($src, $country)
  {
    $hds = $this->getSummaryHds();

    $summary = array_fill_keys(array_keys($hds), 0);
    foreach($src as $recs){
      $wt = $recs['wt'];
      if($wt === 'N/A'){
        $summary['na']++;
      }
      else{
        $intv = (int)ceil($wt / 20.0) * 20;
        $summary[$intv]++;
      }
    }

    foreach($summary as $intv => $sum){
      if($intv !== 'cap' && $sum < 1){
        unset($hds[$intv]);
      }
    }

    $summary['cap'] = '件數';

    $buf = '';
    $buf .= "<h3>$country</h3>";
    $buf .= sprintf('<div>共 %s 件</div>', count($src));

    $tb = new \Gulei\Table\Simple($hds, array($summary));
    $buf .= $tb->theme(null, null, array('attrs' => array('class' => 'summary print')));
    return $buf;
  }

  private function themeSummary()
  {
    $buf = '';
    $bunched = \Gulei\_Array::lambda($this->order_recs, 'bunch', 'country');

    foreach($bunched as $country => $src){
      $buf .= $this->getSummaryTable($src, $country);
    }
    return $buf;
  }
  
  private function themeHeader()
  {
    $src = <<<HTML
<h1>貿隆有限公司</h1>
<p>
325桃園縣龍潭鄉烏林村中豐路779巷10號<br/>
<b>電話</b> 03-4801776, 03-4801778<br/>
<b>傳真</b> 03-4700828<br/>
<b>日期</b> %s
</p>
HTML;

    $Date = new \DateTime();
    return sprintf($src, $Date->format('Y-m-d'));
  }

  public function theme()
  {
    $buf = '';
    $buf .= $this->themeHeader();
    $buf .= $this->themeList();
    $buf .= $this->themeSummary();

    drupal_add_css(drupal_get_path('module', 'oerp_delivery') . '/oerp_delivery.css');
    return $buf;
  }
}