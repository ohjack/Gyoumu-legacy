<?php
namespace Gulei\Table;

class Select extends Arch
{
  protected $filters = array();

  public function __construct($arch_src, $recs, $param = array())
  {
    parent::__construct($arch_src, new \Gulei\Provider\Lambda($recs), $param);
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
      $filter_vals = new \Gulei\_Array\Basic($this->getRecordsAltered());
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
    $Src = new \Gulei\Arch\Xml('<root>' . $eles['#children'] . '</root>');

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
          throw new \Exception('\Gulei\Table\Select: selector snippet not found - ' . $sel);
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