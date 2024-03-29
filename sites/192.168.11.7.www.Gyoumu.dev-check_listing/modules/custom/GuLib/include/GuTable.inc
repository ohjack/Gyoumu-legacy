<?php
use Gulei as G;

require_once 'GuArch.inc';
require_once 'GuBinary.inc';
require_once 'GuFuncTrans.inc';
require_once 'GuFuncValid.inc';
require_once 'processor.inc';
require_once 'command.inc';
require_once 'pager.inc';

abstract class AbstractTable
{
  protected $hds;
  protected $recs;
  protected $rows;
  
  protected $count;
  protected $param;
  protected $marks = array();

  protected $Ele;
  protected $provRecord;
  protected $provHeader;
  public $mods;

  abstract protected function getDefaultParam();
  abstract protected function getFuncPrefix();

  public function __construct($arch_src, G\InterfaceProvider $provHeader, G\InterfaceProvider $provRecord, $param)
  {
    $this->Ele = new GuArchElement($arch_src);
    $this->provHeader = $provHeader;
    $this->provRecord = $provRecord;
    $this->param = $param;

    $this->mods = array(
      'hd' => array(
        'raw' => array(new G\PackHeaderRawMods()),
        'alt' => array(new G\PackHeaderAltMods()),
      ),
      'rec' => array(
        'alt' => array(new G\PackRecordAltMods()),
      ),
      'row' => array(
        'raw' => array(new G\PackRowRawMods()),
        'alt' => array(new G\PackRowAltMods()),
      )
    );
  }

  protected function getMods($domain, $type)
  {
    $mods = array();
    foreach($this->mods[$domain][$type] as $mod){
      if($mod instanceof G\AbstractModPack){
        $mods = array_merge($mods, $mod->getItems());
      }
      else{
        $mods[] = $mod;
      }
    }
    return $mods;
  }
  
  public function getRecordsCount(){
    return $this->provRecord->count();
  }

  public function getRowCount(){
    return $this->count;
  }

  protected function createPager(){
    $param = $this->getParam();
    $param['count'] = $this->getRecordsCount();
    return new G\Pager($param);
  }

  protected function getPager(){
    return $this->createPager()->theme();
  }

  public function getParam($name = null, $override = null){
    $param = $this->getDefaultParam();

    if($this->Ele){
      $param = array_merge(
        $param,
        $this->Ele->getNodeValues('./param[@name]')
      );
    }

    $param = array_merge($param, $this->param);

    if(is_array($override)){
      $param = array_merge($param, $override);
    }

    if(is_null($name)){
      return $param;
    }
    else{
      return $param[$name];
    }
  }
  
  public function isValid(){
    return !count($this->marks['error']);
  }

  protected function getHeaderProcessor($param)
  {
    $Proc = new GuProcessor();
    $Proc
        ->setDataProvider($this->provHeader)
        ->addReference(array('_param' => $param))
        ->addModifiers($this->getMods('hd', 'raw'))
    ;

    if($this->Ele){
      $arch = $this->Ele->dump();
      $Proc
          ->applyArchReference($arch)
      ;
    }
    return $Proc;
  }

  public function getHeaders($hds = null, $param = array()){
    if($hds)
      return $this->hds = $hds;

    $param = $this->getParam(null, $param);
    $output = $this->getHeaderProcessor($param)->getOutput();
    return $this->hds = $output[0]['data'];
  }

  public function getRecords($recs = null, $param = array()){
    if($recs)
      return $this->recs = $recs;

    return $this->provRecord->getData();
  }

  protected function getRowsProcessor($recs, $param)
  {
    $hds = $this->getHeadersAltered($param);
    $Proc = new GuProcessor();
    $Proc
        ->setDataProvider(new G\ProviderCompose($recs, $hds))
        ->addReference(array('_param' => $param))
        ->applyArchReference($this->Ele->dump())
        ->addModifiers($this->getMods('row', 'raw'))
    ;
    return $Proc;
  }

  protected function countRows()
  {
    return count($this->rows);
  }

  public function getRows($recs = null, $rows = null, $param = array()){
    if($rows)
      return $this->rows = $rows;

    $param = $this->getParam(null, $param);
    $recs = $this->getRecords($recs, $param);
    $this->alterRecords($recs);
    $Proc = $this->getRowsProcessor($recs, $param);

    $this->rows = $Proc->getOutput();
    $this->count = $this->countRows();
    return $this->rows;
  }

  protected function alterHeaders(&$hds, $param = array()){
    $param = $this->getParam(null, $param);

    $toAlter = array(
      array('data' => $hds)
    );

    $p = new GuProcessor();
    $p
        ->setDataProvider(new G\ProviderLambda($toAlter))
        ->addReference(array('_param' => $param))
        ->addModifiers($this->getMods('hd', 'alt'))
    ;

    $altered = $p->getOutput();
    $hds = $altered[0]['data'];
  }

  public function getHeadersAltered($param = array()){
    $param = $this->getParam(null, $param);
    $hds = $this->getHeaders(null, $param);
    $this->alterHeaders($hds, $param);
    return $hds;
  }


  protected function searchFunction($type, $param = null){
    $eles = array();
    $eles[] = $this;

    if(isset($param[$type]) && $param[$type]){
      $eles[] = new GuArchElement('<root>' . $param[$type] . '</root>');
    }

    $funcs = array();
    foreach($eles as $ele){
      $funcNode = $ele->xp('./' . $type, $ele->getArch())->item(0);

      if($funcNode){
        foreach($ele->xp('./function | ./func', $funcNode) as $func_ele){
          $func = array();

          if($func_type = $func_ele->getAttribute('type')){
            $func['type'] = $func_type;
          }
          else{
            $func['type'] = 'error';
          }

          foreach($ele->xp('./source | ./src', $func_ele) as $node){
            $newNode = $this->getDoc()->importNode($node, true);
            $func['src'][] = $newNode;
          }

          $func['fld'] = array();
          foreach($ele->xp('./field | ./fld', $func_ele) as $node){
            $func['fld'][] = $node->nodeValue;
          }

          foreach($ele->xp('./message | ./msg', $func_ele) as $node){
            $func['msg'][] = trim($node->nodeValue);
          }

          $ref = array(
            'param' => $param,
            'ObjTable' => $this,
            'func' => $func,
            'prefix' => $this->getFuncPrefix(),
          );

          $classname = 'Gulei\Command' . ucfirst($type);
          $funcs[] = new $classname($ref);
        }
      }
    }
    return $funcs;
  }

  protected function alterRecords(&$recs, $param = array()){
    $this->marks = array();
    $param = $this->getParam(null, $param);
    $param['func_prefix'] = $this->getFuncPrefix();
    $param['ObjTable'] = $this;

    if($this->Ele){
      $param['arch'] = $this->Ele->dump();
    }

    $p = new GuProcessor();
    $p
        ->setDataProvider(new G\ProviderLambda($recs))
        ->addReference(array('_param' =>$param))
        ->addModifiers($this->getMods('rec', 'alt'))
    ;

    $recs = $p->getOutput();
  }

  public function getRecordsAltered($param = array()){
    $param = $this->getParam(null, $param);
    $recs = $this->getRecords(null, $param);
    $this->alterRecords($recs, $param);
    return $recs;
  }

  protected function alterRows(&$rows, $param = array()){
    $param = $this->getParam(null, $param);

    $p = new GuProcessor();
    $p
        ->setDataProvider(new G\ProviderLambda($rows))
        ->applyArchReference($this->Ele->dump())
        ->addReference(array('_param' => $param))
        ->addModifiers($this->getMods('row', 'alt'));

    $rows = $p->getOutput();
  }

  public function getRowsAltered($recs = null, $rows = null, $param = array()){
    $param = $this->getParam(null, $param);
    $rows = $this->getRows($recs, $rows, $param);
    $this->alterRows($rows, $param);
    return $rows;
  }

  public function isSeparatorSet()
  {
    return !!$this->marks['sep'];
  }

  public function markRecord($name, $ind, $msg, $type){
    $this->marks[$type][$ind][$name] = $msg;
  }

  protected function markCells(&$rows, $marked, $type){
    if(!is_array($marked)){
      return;
    }

    foreach($marked as $ind => $each_cell){
      if(isset($rows[$ind])){
        foreach($each_cell as $name => $msg){
          $data = &$rows[$ind]['data'][$name]['data'];
          preg_match('@<span class="gu_value">([\s\S]*)</span>@', $data, $m);

          if(strlen($m[1]) == 0){
            $data = str_repeat('&nbsp;', 8);
          }

          $data = '<span class="' . $type . '">' . $data . '</span>';
          if($msg){
            $data = sprintf('<span>%s<span class="show-tooltip-text">%s</span></span>', $data, $msg);
          }
        }
      }
    }
  }

  public function theme($hds = null, $rows = null, $param = array()){
    $param = $this->getParam(null, $param);

    $hds = $this->getHeaders($hds, $param);
    $rows = $this->getRows(null, $rows, $param);

    $this->alterHeaders($hds, $param);
    $this->alterRows($rows, $param);

    if($param['mark_invalid']){
      $this->markCells($rows, $this->marks['error'], 'invalid');
    }

    if($param['mark_notice']){
      $this->markCells($rows, $this->marks['notice'], 'notice');
    }

    if($param['mark_translated'] && $trans = $this->marks['translated']){
      $this->markCells($rows, $trans, 'translated');
    }

    if(is_array($this->marks['sep'])){
      $seps = array_keys($this->marks['sep']);

      for($ind = array_shift($seps); $ind < count($rows); $ind++){
        $rows[$ind]['class'] .= ' sep-after';
      }
    }

    $attrs = $param['attrs'];
    $attrs['class'] .= ' ' . $param['class'] . ' gu-table';
    $attrs['style'] .= $param['style'];

    if($param['copy_method'] == 'merge'){
      $attrs['class'] .= ' merge';
    }
    if($param['hidetip']){
      $attrs['class'] .= ' hidetip';
    }

    G\IncluderTable::includeCss();
    G\IncluderTable::includeJs();

    if(empty($rows)){
      $rows = array(array());
    }

    $output = theme('table', $hds, $rows, $attrs);

    if($param['sticky_header'] === false){
      $Output = new GuArchElement($output);
      $Output->xp('/table/thead')->item(0)->setAttribute('class', 'tableHeader-processed');
      return $Output->dump();
    }

    if($param['pager']){
      $output .= $this->getPager();
    }

    return $output;
  }

  public function mapRecords($mapping){
    $recs = new GuArray($this->getRecordsAltered());
    return $recs->mapping($mapping);
  }

  public function getCsvFile($param = null, $name = ''){
    $param = is_null($param) ? array() : $param;

    $param = array_merge(
      array(
        'id' => true,
        'copy_method' => 'merge',
        'nl2br' => false,
      ),
      $param
    );

    $param = $this->getParam(null, $param);
    $recs = $this->getRecordsAltered($param);

    $path = sprintf('%s/%s_%s.csv', file_directory_temp(), $name, date('m-d'));
    return './' . file_save_data($this->dumpCsv($recs, $param), $path);
  }

  public function dumpCsv($recs = null, $param = array()){
    $func_prepareRow = create_function('$i', '
      $val = is_array($i) ? $i["data"] : $i;
      return "\"" . str_replace("\"", "\"\"", $val) . "\"";
    ');

    $param = $this->getParam(null, $param);
    $mani = new ManipulatorGuValue();

//  set headers
    $hds = $this->getHeaders(null, $param);
    foreach($hds as &$hd){
      $val = (isset($hd['data'])) ? $hd['data'] :$hd;
      $hd = $mani->value($val);
    }
    $hds = array_map($func_prepareRow, $hds);

    $buffer = implode(',', $hds) . "\n";

//  set records
    if(is_null($recs)){
      $recs = $this->getRecordsAltered($param);
    }

    foreach($recs as &$rec){
      $rec = array_map($func_prepareRow, $rec);

      $seq_rec = array();
      foreach(array_keys($hds) as $name){
        $seq_rec[$name] = $mani->value($rec[$name]);
      }
      $rec = implode(',', $seq_rec);
    }

    $buffer .= implode("\n", $recs);
    return $buffer;
  }

  public function save()
  {
    $this->Ele->save();
    return $this;
  }

  public function renew()
  {
    $this->Ele->renew();
    return $this;
  }

  public function __call($name, $args)
  {
    return ($this->Ele)
        ? call_user_func_array(array($this->Ele, $name), $args)
        : null;
  }
}

class GuTable extends AbstractTable
{
  protected function getDefaultParam()
  {
    return array();
  }

  protected function getFuncPrefix()
  {
    return array('GuFunc_');
  }
}

class GuArchTable extends AbstractTable
{
  function __construct($arch_src, $provRecs, $param = array()){
    parent::__construct(
      $arch_src,
      new G\ProviderArch($arch_src),
      $provRecs,
      $param
    );
  }

  protected function getDefaultParam(){
    return array(
      'mark_translated' => true,
      'mark_invalid' => true,
      'mark_notice' => true,
      'mark_missing' => true,
      'copy_method' => 'ditto',
      'nl2br' => true,
      'sticky_header' => true,
    );
  }
  
  protected function getFuncPrefix()
  {
    return array('GuFunc_');
  }

  protected function includeCssJs()
  {
    drupal_add_css(drupal_get_path('module', 'GuLib') . '/css/table.css');
    drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuTable.js');
  }
}

class GuSelectTable extends GuArchTable
{
  protected $filters = array();

  public function __construct($arch_src, $recs, $param = array())
  {
    parent::__construct($arch_src, new G\ProviderLambda($recs), $param);
  }

  protected function alterHeaders(&$hds, $param = array()){
    parent::alterHeaders($hds, $param);
    $param = $this->getParam(null, $param);

    if(is_null($this->filter)){
      $this->filter = array();
      
      foreach($this->xp('/tree/field') as $fld){
        $name = $fld->getAttribute('name');
        foreach($this->xp('./param', $fld) as $param){
          switch($param->getAttribute('name')){
            case 'filter':
              $this->filters[] = $name;
              break;
          }
        }
      }
    }
  }
  
  public function getFormEles(){
    return array(
      '#type' => 'GuSelectTable',
      '#obj' => $this->save(),
    );
  }
  
  protected function getSelectionValue($rec, $ind){
    return $ind;
  }
  
  public function getSelectionEles($ele){
    $form = array();
    $type = $this->getParam('type');

    switch($type){
      case 'select-one':
        $opts = array();
        
        foreach($this->getRecords() as $ind => $rec){
          if(!in_array('_DITTO_', $rec, true)){
            $sel = $this->getSelectionValue($rec, $ind);
            $opts[$sel] = ''; 
          }
        }
        
        $opts['-1'] = '-1';
                        
        $form['sel'] = array(
          '#type' => 'radios',
          '#options' => $opts,
        );        
        break;
        
      case 'select-many':
        $ele_tpl = array(
          '#type' => 'checkbox', 
        );

        foreach($this->getRecords() as $ind => $rec){
          if(!in_array('_DITTO_', $rec, true)){
            $sel_id = $this->getSelectionValue($rec, $ind);
            $name = 'sel-' . $sel_id;
            
            $form[$name] = $ele_tpl;
            $form[$name]['#attributes']['sel_id'] = $sel_id; 
          }
        }
        break;
    }

    return $form;
  }
  
  public function theme($eles, $hds = null, $rows = null, $param = array()){
    $param = $this->getParam(null, $param);

    if(is_null($eles)){
      return parent::theme($hds, $rows, $param);  
    }

    $hds = $this->getHeaders($hds, $param);
    $this->alterHeaders($hds, $param);

    $recs = $this->getRecords();
    $rows = $this->getRows(null, $rows, $param);

    $param['attrs']['class'] .= ' gu-seltable';

    if(!$param['type'] || $rows[0]['empty']){
      return parent::theme($hds, $rows, $param); 
    }
    
//  prepare headers with selector  
    switch($param['type']){
      case 'select-many':
        $op = '<input type="checkbox"/>';
        break;
      case 'select-one':
        $op = '';
        break;
    }
    
    $hds = array_merge(array('%sel' => array(
      'data' => $op,
      'class' => 'selector',
    )), $hds);
    
// prepare headers with filter
    foreach($this->filters as $name){
      $filter_vals = new GuArray($this->getRecordsAltered());
      $filter_vals = $filter_vals->collect($name);
      sort($filter_vals);

      $filter_opts = array('--FILTER--');

      foreach($filter_vals as $filter_val){
        if($filter_val != '_DITTO_'){
          $filter_opts[$filter_val] = (strlen(trim($filter_val)) < 1)
              ? '&lt;blank&gt;' : $filter_val;
        }
      }
      
      $filter_src = '';

      foreach($filter_opts as $val => $filter_opt){
        $filter_src .= sprintf('<option value="%s">%s</option>',
          $val, $filter_opt);
      }
      
      $filter_src = '<select class="GuSelectTable filter">' . $filter_src . '</select>';
      $hds[$name]['data'] = $hds[$name]['data'] . '<br/>' . $filter_src;
    }
    
//  prepare rows with selector  
    $Src = new GuArchElement('<root>' . $eles['#children'] . '</root>');

    if($rows['data']){
      $it = &$rows['data'];
    }
    else{
      $it = &$rows;
    }

    $snip_name = implode('-', $eles['#parents']);
    $snip_name = str_replace('_', '-', $snip_name);

    foreach($it as $ind => &$row){
      if($row['merged']){
        for($c = $ind - 1; $c >= 0 && $it[$c]['merged']; $c--);

        if(!isset($it[$c]['data']['%sel']['rowspan'])){
          $it[$c]['data']['%sel']['rowspan'] = 1;
        }
        $it[$c]['data']['%sel']['rowspan']++;
      }
      else{
        $sel = $this->getSelectionValue($recs[$ind], $ind);

        switch($param['type']){
          case 'select-many':
            $snip = $Src->xp(
              '//input[@sel_id="' . $sel . '"]')
              ->item(0);
            break;
            
          case 'select-one':
            $snip = $Src->xp(
              '//input[@id="edit-' . $snip_name . '-sel-' . $sel . '"]')
              ->item(0);
            break;
        }
        
        if(!$snip){
          dpr($sel);
          dpr($Src->dump());
          echo '<pre>';
          throw new Exception('GuSelectTable: selector snippet not found.');
        }
        
        $ex = array('%sel' => array(
          'data' => $Src->dump($snip),
          'class' => 'selector',
        ));
        
        if($row['data']){
          $row['data'] = array_merge($ex, $row['data']);
        }
        else{
          $row = array('data', array_merge($ex, $row));
        }
      }
    }
    
    return parent::theme($hds, $rows, $param);
  }
}

function GuLib_GuSelectTable_process($ele, $edit, &$form_state, $form){
  $obj = $ele['#obj'];
  $obj->renew();

  $ele = array_merge($ele, $obj->getSelectionEles($ele));
  return $ele;
}

function theme_GuSelectTable($eles){
  drupal_add_js(drupal_get_path('module', 'GuLib') . '/js/GuSelectTable.js');
  $obj = $eles['#obj']->renew();
  
  $gu_name = $eles['#parents'][count($eles['#parents']) - 1];
  
  return sprintf('<div gu_name="%s">%s</div>',
    $gu_name, $obj->theme($eles));
}
