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

      $chk = new FedprintCheckPrint($print_rec->rid);
      $resp = $chk->getResp();

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

class FedprintCheckPrint
{
  private $rid;
  private $rec;
  private $out;

  public function __construct($rid)
  {
    $this->rid = $rid;
    $this->rec = $this->getRec();
  }

  private function getRec()
  {
    return _fedprint_getRec($this->rid);
  }

  private function parseOutfile($resp)
  {
    preg_match_all('/(\d*),"(.*)"/U', $resp, $m);

    $vals = array();
    foreach($m[1] as $ind => $val){
      $vals[] = array('code' => $val, 'value' => $m[2][$ind]);
    }
    return $vals;
  }

  private function loadOutfile()
  {
    $path = variable_get('fedprint_path', '');

    if(!$path){
      drupal_set_message('FedPrint: File path have not been set.', 'error');
      return array('success' => false, 'msg' => 'Error: File path not found.');
    }

    $files = glob(sprintf('%s/out/gyoumu_batch-%s.out', $path, $this->rid));

    if($files[0]){
      $this->rec->resp = file_get_contents($files[0]);
      $m = $this->parseOutfile($this->rec->resp);
      $this->rec->tid = $m[2][1];

      drupal_write_record('fedprint_recs', $this->rec, 'rid');
      return true;
    }
    return false;
  }

  private function searchOutfile($code)
  {
    if(!$this->out){
      $this->out = $this->parseOutfile($this->rec->resp);
    }

    foreach($this->out as $val){
      if($val['code'] == $code){
        return $val['value'];
      }
    }
    return false;
  }

  public function getResp()
  {
    if(!$this->rec->resp && !$this->loadOutfile()){
      return array(
        'success' => null,
        'msg' => 'Error: No response yet. Try to reload this page.',
      );
    }

    if(($tnum = $this->searchOutfile('1123')) !== false){
      if(is_null($this->rec->tid) || is_null($this->rec->tnum)){
        $this->rec->tid = $this->searchOutfile('1');
        $this->rec->tnum = $tnum;
        drupal_write_record('fedprint_recs', $this->rec, 'rid');
      }

      return array(
        'success' => true,
        'msg' => 'Printed: ' . $tnum,
        'date' => $this->searchOutfile('24'),
        'tid' => $this->rec->tid,
        'tnum' => $$tnum,
      );
    }

    if(($error = $this->searchOutfile('3')) !== false) {
      return array('success' => false, 'msg' => 'Error: ' . $error);
    }
  }
}

?>
