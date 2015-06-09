<?php

module_load_include('inc', 'GuLib', 'include/GuTable');

class FedPrintOverview extends GuArchTable
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

    parent::__construct($arch, null);
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

  public function getRecordsCount(){
    $sql = <<<SQL
SELECT COUNT(*) FROM {fedprint_recs}
WHERE status > 0
SQL;

    return db_result(db_query($sql));
  }
  
  public function getRecords()
  {
    $params = $this->getParam();

    $sql = '
      SELECT * FROM {fedprint_recs}
      WHERE status > 0
      ORDER BY rid DESC
    ';

    $rs = pager_query(
      $sql,
      $params['items_per_page'], 0,
      'SELECT COUNT(*) FROM {fedprint_recs} WHERE status > 0'
    );

    $print_recs = array();
    while ($data = db_fetch_object($rs)) {
      $print_recs[] = $data;
    }

    $ccode = _fedprint_getCountryCodeOpts();

    $recs = array();
    foreach ($print_recs as $print_rec) {
      $rec = array();
      $src = unserialize($print_rec->src);

      $rec['rid'] = $print_rec->rid;
      $rec['tid'] = $src['tid'];
      $rec['ref'] = $src['ref_note'];
      $rec['name'] = $src['recp_contact'];
      $rec['country'] = $ccode[$src['recp_country_code']];

      $resp = fedprint_check_print($print_rec->rid);
      $rec['date'] = $resp['date'];
      $rec['status'] = $resp['msg'];

      $recs[] = $rec;
    }

    $errors = array();
    foreach ($recs as $rec) {
      if (!$rec['date']) {
        $errors[] = $rec;
      }
    }

    $recs = array_filter(
      $recs,
      create_function(
        '$i', 'return !empty($i["date"]);'
      )
    );

    $recs = array_merge($errors, $recs);
    return $recs;
  }
}

?>