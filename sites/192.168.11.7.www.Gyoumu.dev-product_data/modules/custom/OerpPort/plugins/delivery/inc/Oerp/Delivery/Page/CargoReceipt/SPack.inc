<?php
namespace Oerp\Delivery\Page\CargoReceipt;

class SPack extends \Gulei\Page\_Abstract
{
  private $carrs;
  private $order_recs;

  const STR_TOWEIGHT = '<span class="to_weight"></span>';
  const STR_NOWEIGHT = '<span class="no_weight"></span>';
  const STR_INVALID = '＊超過限制＊';

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

  protected function checkPkgValid($pkg) {
    if ($pkg['weight'] >= 2000
        || ($pkg['length'] + $pkg['width'] + $pkg['height']) >= 90
        || $pkg['length'] >= 60
        || $pkg['width'] >= 60
        || $pkg['height'] >= 60
    ) {
      return FALSE;
    }
    return TRUE;
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

    $ids = \Gulei\_Array::lambda($orders, 'collect', 'id');
    $skus = \Gulei\_Array::lambda($orders, 'collect', 'move_lines.product_id.default_code');

    $GetPkgType= new \Oerp\Delivery\Helper\GetPkgType($skus, $ids);
    $order_recs = array();
    $order_count = count($orders);

    for ($ind = 0, $count = 0; $ind < $order_count; $ind++) {
      $order = $orders[$ind];
      $combined = !$orders[$ind + 1]['id'] && $orders[$ind + 1]['move_lines.id'];

      $id = $order['id'];
      $qty = $order['move_lines.product_qty'];

      if ($order['id']) {
        $pkg = $GetPkgType->get($id);

        $newOrder = array(
          'count' => ++$count,
          'packid' => $order['name'],
          'tnum' => $order['x_tnum'],
          'qty' => 1,
          'buyer' => $order['address_id.name'],
          'country' => $order['address_id.state_id.country_id'],
        );

        /* Combined or multi-item need to weight */
        if ($qty > 1 || $combined) {
          $newOrder['wt'] = self::STR_TOWEIGHT;
        }

        /* Missing suitable packaging */
        else if (!$pkg['weight']) {
          $newOrder['wt'] = self::STR_NOWEIGHT;
        }

        /* Show real weight */
        else if ($this->checkPkgValid($pkg)) {
          $newOrder['wt'] = $pkg['weight'];
        }

        /* Packaging oversized */
        else {
          $newOrder['wt'] = self::STR_INVALID;
        }
        $order_recs[] = $newOrder;
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

    for($c = 20; $c <= 2000; $c += 20){
      $hds[$c] = $c;
    }
    return$hds;
  }

  protected function getListBlockSrc($list, $title) {
    $buf = '';
    $buf .= "<h3>$title</h3>";

    $list_src = implode('<br/>', $list);
    $list_src = '<p style="font-size: small;">' . $list_src . '</p>';
    $buf .= $list_src;

    return $buf;
  }

  private function getSummaryTable($src, $country)
  {
    $hds = $this->getSummaryHds();
    $summary = array_fill_keys(array_keys($hds), 0);
    $count = 0;

    foreach($src as $rec){
      $wt = $rec['wt'];

      if ($wt === self::STR_NOWEIGHT || $wt === self::STR_TOWEIGHT){
        $summary['na']++;
      }
      elseif ($wt === self::STR_INVALID) {
        continue;
      }
      else {
        $intv = (int)ceil($wt / 20.0) * 20;
        $summary[$intv]++;
      }
      $count++;
    }

    foreach($summary as $intv => $sum){
      if($intv !== 'cap' && $sum < 1){
        unset($hds[$intv]);
      }
    }

    $summary['cap'] = '件數';

    $buf = '';
    $buf .= "<h3>$country</h3>";
    $buf .= sprintf('<div>共 %s 件</div>', $count);

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

    $statuss = array(
      self::STR_TOWEIGHT,
      self::STR_NOWEIGHT,
      self::STR_INVALID,
    );

    $ex_summary = array();

    foreach ($this->order_recs as $rec) {
      foreach ($statuss as $status) {
        if ($ex_summary[$status] === NULL) {
          $ex_summary[$status] = array();
        }

        if ($rec['wt'] === $status) {
          $ex_summary[$status][] = $rec['packid'];
        }
      }
    }

    $ex_buf = '';
    $ex_buf .= $this->getListBlockSrc($ex_summary[self::STR_NOWEIGHT], '缺包裝');
    $ex_buf .= $this->getListBlockSrc($ex_summary[self::STR_TOWEIGHT], '待測');
    $ex_buf .= $this->getListBlockSrc($ex_summary[self::STR_INVALID] ,'超過限制');

    $group = array(
      'ex_summary' => array(
        '#title' => '+',
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        'content' => array(
          '#type' => 'markup',
          '#value' => $ex_buf,
        )
      )
    );

    $buf .= drupal_render($group);
    return $buf;
  }
  
  private function themeHeader()
  {
    $src = <<<HTML
<h1>迅匯有限公司</h1>
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