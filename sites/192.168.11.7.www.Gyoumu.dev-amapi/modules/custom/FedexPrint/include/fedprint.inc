<?php

module_load_include('inc', 'GuLib', 'include/GuTable');

class FedPrintOverview extends \Gulei\Table\Arch
{
  function __construct(){
    $arch = <<<XML
<tree>
  <field name="date" string="Date"/>
  <field name="tid" string="T.ID"/>
  <field name="ref" string="Ref."/>
  <field name="name" string="Recipient"/>
  <field name="country" string="Country"/>
  <field name="status" string="Status"/>
  <field name="func:action" string="Action"/>

  <validate>
    <func>
      <src>
        <![CDATA[
          \$pat = '/^Printed:/';
          return preg_match(\$pat, \$rec[\$name]);
        ]]>
      </src>
      <fld>status</fld>
    </func>
  </validate>

  <translate>
    <func>
      <src>
        <![CDATA[
          return sprintf(
            '<div>%s | %s</div>',
            l('review', 'fedprint/record/create/' . \$rec['rid']),
            l('delete', 'fedprint/record/delete/' . \$rec['rid'])
          );
        ]]>
      </src>
      <fld>func:action</fld>
    </func>
  </translate>
</tree>
XML;

    parent::__construct($arch, new \Fedex\Provider\PrintOverview());
  }

  public function getDefaultParam(){
    return array(
      'mark_invalid' => true,
      'pager' => true,
      'items_per_page' => 100,
    );
  }

  public function getHeaders(){
    foreach($this->extractAttr('string', 'name') as $name => $str){
      $hds[$name] = array(
        'data' => $str,
      );

      if($name == 'func:action'){
        $hds[$name]['style'] = 'width: 80px;';
      }
    }
    return $hds;
  }
}

?>