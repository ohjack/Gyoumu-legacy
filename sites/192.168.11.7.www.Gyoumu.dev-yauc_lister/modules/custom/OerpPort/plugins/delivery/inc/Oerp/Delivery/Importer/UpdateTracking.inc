<?php
namespace Oerp\Delivery\Importer;

class UpdateTracking extends \Gulei\Importer\Arch\_Abstract {
  protected function getClassname() {
    return __CLASS__;
  }

  public function getArch() {
    return <<<XML
<tree>
  <field name="id"/>
  <field name="Ref."/>
  <field name="Carrier"/>
  <field name="Track #"/>

  <validate>
    <func>
      <src name="cmd:isEmpty" not="1"/>
      <fld>id</fld>
      <msg>Missing ID.</msg>
    </func>
    <func>
      <src name="cmd:isUnique"/>
      <fld>Track #</fld>
      <msg>Duplicate Track #</msg>
    </func>
  </validate>
</tree>
XML;
  }

  public function submit($form, &$form_state) {
    $recs = $this->getRecs($form);
    $q = new \Oerp\Query\Basic('stock.picking');

    foreach($recs as $rec){
      $q->setIds($rec['id']);

      if(!empty($rec['Track #'])){
        $q->write(array('x_tnum' => $rec['Track #']));
      }
    }
    drupal_set_message(sprintf('%d tracking number(s) updated.', count($recs)));
  }
}