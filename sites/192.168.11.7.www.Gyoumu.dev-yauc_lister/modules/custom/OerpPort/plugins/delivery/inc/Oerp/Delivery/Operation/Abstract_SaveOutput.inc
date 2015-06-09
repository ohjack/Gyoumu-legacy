<?php
namespace Oerp\Delivery\Operation;

abstract class Abstract_SaveOutput extends \Oerp\Delivery\Operation\Abstract_InitDate {
  const DATE_FORMAT = 'Y-m-d H:i:s';

  protected function getTimeStartStr() {
    return date(self::DATE_FORMAT, $this->getTimeStart());
  }

  protected function getTimeEndStr() {
    return date(self::DATE_FORMAT, $this->getTimeEnd());
  }

  protected function getQueryArch() {
    return <<<XML
<tree name="view">
  <param name="oerp:model">stock.picking</param>

  <field name="id"/>
  <field name="x_seller" string="Seller"/>
  <field name="name" string="Ref."/>

  <field name="date" string="O.Date"/>
  <field name="date_done"/>

  <field name="origin" string="O.ID"/>
  <field name="move_lines">
    <field name="product_id">
      <field name="default_code" string="SKU"/>
    </field>
    <field name="product_qty" string="Qty"/>
  </field>

  <field name="address_id" select="1">
    <field name="name" string="Buyer"/>

    <field name="state_id">
      <field name="country_id" string="Country"/>
    </field>
  </field>

  <field name="x_cust_val" string="C.Value"/>
  <field name="carrier_id" string="Carrier"/>
</tree>
XML;
  }

  protected function getRecs() {
    $cri = array(
      array('type', '=', 'delivery'),
      array('state', '=', 'done'),
      array('date', '>=', $this->getTimeStartStr()),
      array('date', '<=', $this->getTimeEndStr()),
    );

    $param = array(
      'oerp:criteria' => $cri
    );

    $arch = $this->getQueryArch();
    $q = new \Oerp\Query\Nested($arch, $param);
    return $q->sum();
  }

  protected function clear() {
    $sql = "DELETE FROM {oerp_delivery_output} WHERE `date_create` >= %d AND `date_create` <= %d";
    return db_query($sql, $this->getTimeStart(), $this->getTimeEnd());
  }

  protected function save() {
    $recs = $this->getRecs();

    foreach($recs as $rec) {
      $rec_tpl = array(
        'rid' => $rec['id'],
        'date_create' => strtotime($rec['date']),
        'date_done' => strtotime($rec['date_done']),
        'seller' => $rec['x_seller'],
        'carrier' => $rec['carrier_id'],
        'country' => $rec['address_id']['state_id']['country_id'],
      );

      foreach($rec['move_lines'] as $line) {
        $new_rec = array_merge($rec_tpl, array(
          'sku' => $line['product_id']['default_code'],
          'qty' => $line['product_qty'],
        ));

        $new_rec = (object) $new_rec;
        drupal_write_record('oerp_delivery_output', $new_rec);
      }
    }
  }

  protected function isSaved() {
    $sql = "SELECT `id` FROM {oerp_delivery_output} WHERE `date_create` >= %d AND `date_create` <= %d";
    $rs = db_query($sql, $this->getTimeStart(), $this->getTimeEnd());
    return $rs->num_rows;
  }

  public function run() {
    if ($this->override) {
      $this->clear();
      $this->save();
    }
    elseif (!$this->isSaved()) {
      $this->save();
    }
  }
}