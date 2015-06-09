<?php
namespace Oerp\Delivery\Helper;

class GetPkgType {
  protected $skus;
  protected $ids;

  public function __construct($skus = array(), $ids = array()) {
    $this->skus = $skus;
    $this->ids = array_values(array_filter($ids));
  }

  protected function getPkgs() {
    $arch = <<<XML
<tree>
  <param name="oerp:model">product.product</param>

  <field name="default_code"/>
  <field name="packaging">
    <field name="ul">
      <field name="name"/>
    </field>
  </field>
</tree>
XML;

    $q = new \Oerp\Query\Nested(
      $arch,
      array('criteria' => array(array('default_code', 'in', $this->skus)))
    );
    return $q->sum()->bunch('default_code');
  }

  protected function getRemoteRecs() {
    $arch = <<<XML
<tree>
  <param name="oerp:model">stock.picking</param>
  <field name="move_lines">
    <field name="product_id">
      <field name="default_code"/>
      <field name="packaging">
        <field name="ul">
          <field name="name"/>
          <field name="x_length"/>
          <field name="x_width"/>
          <field name="x_height"/>
        </field>
        <field name="length"/>
        <field name="width"/>
        <field name="height"/>
        <field name="weight"/>
      </field>
    </field>
    <field name="product_qty"/>
  </field>
  <field name="carrier_id"/>
</tree>
XML;

    $q = new \Oerp\Query\Nested($arch, array('oerp:ids' => $this->ids));
    return $q->sum()->promote('id');
  }

  protected function pickPkg($pkg, $seqs) {
    foreach ($seqs as $seq) {
      foreach ($pkg as $ul) {
        $pkgname = $ul['ul']['name'];

        switch ($seq) {
          case 'fedex':
            if (preg_match('/^fedex /i', $pkgname)) {
              return $ul;
            }
            break;

          case 'box':
            if (preg_match('/ box/i', $pkgname)
                && !preg_match('/^fedex /i', $pkgname)
            ) {
              return $ul;
            }
            break;

          case 'bag':
            if (preg_match('/ bag/i', $pkgname)) {
              return $ul;
            }
            break;
        }
      }
    }
    return FALSE;
  }

  protected function getNaPkg() {
    return array(
      'name' => 'N/A',
      'length' => FALSE,
      'width' => FALSE,
      'height' => FALSE,
      'weight' => FALSE,
    );
  }

  public function get($id) {
    static $remote_recs = NULL;

    if ($remote_recs === NULL) {
      $remote_recs = $this->getRemoteRecs();
    }

    $remote_rec = $remote_recs[$id];

    if (count($remote_rec['move_lines']) > 1
        || $remote_rec['move_lines'][0]['product_qty'] > 1
    ){
      return $this->getNaPkg($remote_rec);
    }
    else {
      $pkg = $remote_rec['move_lines'][0]['product_id']['packaging'];

      switch ($remote_rec['carrier_id']) {
        case 'Fedex IE':
        case 'Fedex IP':
          $pkg = $this->pickPkg($pkg, array('fedex', 'box'));
          break;

        case 'S.Pack':
        case 'R.Mail':
        case 'Air Parcel':
        case '郵寄掛號':
          $pkg = $this->pickPkg($pkg, array('bag', 'box'));
          break;

        default:
          $pkg = $this->pickPkg($pkg, array('box', 'bag'));
          break;
      }
    }

    if ($pkg === FALSE) {
      return $this->getNaPkg();
    }
    else {
      $pkg_send = array();
      $pkg_send['name'] = $pkg['ul']['name'];
      $pkg_send['weight'] = $pkg['weight'];

      foreach (array('length', 'height', 'width') as $dim) {
        if (!($dim_val = $pkg['ul']['x_' . $dim])) {
          $pkg_send[$dim] = $pkg[$dim];
        }
        else {
          $pkg_send[$dim] = $dim_val;
        }
      }
      return $pkg_send;
    }
  }
}