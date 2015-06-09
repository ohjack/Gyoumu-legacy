<?php 

include_once('lib/PHPTAL.php');
module_load_include('inc', 'GuLib', 'include/GuArray');
module_load_include('inc', 'GuLib', 'include/GuCsv');

class ChteTemplate
{
  private $csvTemplate;
  private $csvSrc;
  private $xhtmlTheme;
  private $html_col;
  protected $nodeTheme;
  protected $nodeCsvTemplate;

  function __construct($theme_id, $tpl_id, $src_path)
  {
    $this->nodeTheme = node_load($theme_id);
    $this->xhtmlTheme = $this->nodeTheme->body;

    $this->nodeCsvTemplate = node_load($tpl_id);
    $this->csvTemplate = new GuCsv(
      array(
           'source' => $this->nodeCsvTemplate->body,
      )
    );

    $this->html_col = $this->nodeCsvTemplate->field_chte_htmlmapping[0]['value'];
    $this->csvSrc = new GuCsv($src_path);
  }

  static public function getThemeOpts()
  {
    $result = db_query("SELECT nid, title FROM {node} WHERE type='%s'", 'chte_theme');
    $themes = array();
    while ($theme = db_fetch_object($result)) {
      $themes[$theme->nid] = $theme->title;
    }
    asort($themes);
    return $themes;
  }

  static public function getTemplateOpts()
  {
    $result = db_query("SELECT nid, title FROM {node} WHERE type='%s'", 'chte_csv');
    $csvs = array();
    while ($csv = db_fetch_object($result)) {
      $csvs[$csv->nid] = $csv->title;
    }
    asort($csvs);
    return $csvs;
  }

  public function getPhptal()
  {
    $phptal = new PHPTAL();
    $phptal->setSource($this->xhtmlTheme);
    return $phptal;
  }

  public function getHeaders()
  {
    return $this->csvTemplate->getHeaders();
  }

  public function getDefaults()
  {
    if (!$this->csvTemplate->getRow(0)) {
      return array_fill_keys(
        $this->csvTemplate->getHeaders(), null);
    }

    return array_combine(
      $this->csvTemplate->getHeaders(),
      $this->csvTemplate->getRow(0)
    );
  }

  public function getOutputTemplateRecs()
  {
    return $this->csvTemplate->connect();
  }

  public function getInputRecs()
  {
    return $this->csvSrc->connect();
  }

  public function getRecords()
  {
    preg_match('/([^-]*)-/', $this->nodeCsvTemplate->title, $m);

    $input = $this->getInputRecs();
    $defaults = $this->getDefaults();
    $headers = $this->csvSrc->getHeaders();

    switch ($m[1]) {
      case 'ebay':
        $proc = new ChteEbayRecProcessor($input, $defaults, $headers);
        break;

      case 'yahoo':
        $proc = new ChteYahooRecProcessor($input, $defaults, $headers);
        break;

      default:
        $proc = new ChteRecProcessor($input, $defaults, $headers);
    }

    return $proc->getRecords();
  }

  public function getAppliedRecs()
  {
    $recs = $this->getRecords();

    foreach ($recs as &$rec) {
      $html = $this->getHtmlByPhptal($rec);
      $html = $this->compressHtml($html);
      $rec[$this->html_col] = $html;
    }
    return $recs;
  }

  public function getAppliedCsv()
  {
    $appliedRecs = $this->getAppliedRecs();
    $hds = $this->getHeaders();
    $mask = array_fill_keys($hds, null);

    $recs = array();
    $recs[] = $hds;

    foreach ($appliedRecs as $appliedRec) {
      $recs[] = array_intersect_key($appliedRec, $mask);
    }

    $a = new GuArray($recs);

    preg_match('/\(charset=([^\)]*)\)/', $this->nodeCsvTemplate->title, $m);
    return $a->dumpCsv($m[1]);
  }

  private function getHtmlByPhptal($input)
  {
    $tpl = $this->getPhptal();
    $tpl->title = $input['Title'];

    foreach ($input as $key => $val) {
      $var_name = str_replace(' ', '_', $key);

      if(strpos($var_name, '_') !== 0){
        $tpl->$var_name = $val;
      }
    }
    return $tpl->execute();
  }

  protected function compressHtml($src)
  {
    $src = str_replace(array("\r\n", "\r"), "\n", $src);
    $lines = explode("\n", $src);
    $new_lines = array();

    foreach ($lines as $i => $line) {
      if (!empty($line))
        $new_lines[] = trim($line);
    }
    return implode($new_lines);
  }

  static public function getTypeName($type)
  {
    $map = array(
      'chte_csv' => 'CHTE CSV',
      'chte_theme' => 'CHTE Theme',
    );
    return $map[$type];
  }
}

class ChteRecProcessor
{
  protected $input;
  protected $defaults;
  protected $headers;

  function __construct($input, $defaults, $headers)
  {
    $this->input = $input;
    $this->defaults = $defaults;
    $this->headers = $headers;
  }

  public function getRecords()
  {
    $recs = array();
    foreach ($this->input as $each_input) {
      $src = array();
      foreach ($each_input as $key => $val) {
        $val = trim($val);

        if (strlen($val)) {
          $src[$key] = $val;
        }
      }

      $src = array_merge($this->defaults, $src);
      $rec = array();

      foreach ($this->headers as $hd) {
        if (!key_exists($hd, $rec)) {
          $rec[$hd] = '';
        }
      }

      foreach ($src as $key => $val) {
        $rec[$key] = $val;
        $rec[$key . '_LOWERCASE'] = strtolower($val);
        $rec[$key . '_UPPERCASE'] = strtoupper($val);
      }

      foreach ($rec as $key => $val) {
        $lines = explode("\n", $val);
        $lines = array_map('trim', $lines);
        $lines = array_filter($lines, 'strlen');
        $rec[$key . '_LINES'] = $lines;
      }

      $recs[] = $rec;
    }
    return $recs;
  }
}

class ChteEbayRecProcessor extends ChteRecProcessor
{
  public function getRecords()
  {
    foreach ($this->input as &$each_input) {
      $each_input['CustomLabel'] = $each_input['Custom Label'];
    }
    return parent::getRecords();
  }
}

class ChteYahooRecProcessor extends ChteRecProcessor
{
  public function getRecords()
  {
    foreach ($this->input as $ind => &$row) {
      $pos_count = 0;

      foreach ($row as $hd => &$cell) {
        switch ($hd) {
          case 'POS_lv1':
          case 'POS_lv2':
          case 'POS_lv3':
          case 'POS_lv4':
          case 'POS_lv5':
            $cell = $cell ? $cell : '1';
            break;

          case 'menu_lv1':
          case 'menu_lv2':
          case 'menu_lv3':
          case 'menu_lv4':
          case 'menu_lv5':
            if (!$cell) {
              $cell = $pos_count % 2
                  ? 'OFF' : 'ON';

              $pos_count++;
            }
            break;
        }
      }
    }

    return parent::getRecords();
  }
}

?>