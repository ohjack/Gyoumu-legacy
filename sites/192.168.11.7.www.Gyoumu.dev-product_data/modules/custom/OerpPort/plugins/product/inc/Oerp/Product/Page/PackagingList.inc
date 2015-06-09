<?php
namespace Oerp\Product\Page;

class PackagingList extends \Gulei\Page\Abstract_Content
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  private function getPackagings()
  {
    $q = new \Oerp\Query\Basic(
      'product.packaging',
      null,
      array('product_id', 'weight', 'length', 'width', 'height', 'ul')
    );

    $recs = $q->raw(FALSE, TRUE);
    $refDim = $this->getDimensionRef();

    $pats = array(
      'fedex' => '/^fedex/i',
      'non-fedex' => '/^(?!fedex)/i'
    );

    $pkgs = array();
    foreach($recs as $rec){
      preg_match('/\[([^\[]*)\]/', $rec['product_id'][1], $m);
      $sku = $m[1];

      if(!isset($pkgs[$sku])){
        $pkgs[$sku] = array();
      }

      unset($pkg);
      $pkg = &$pkgs[$sku];

      foreach($pats as $pname => $pval){
        if(preg_match($pval, $rec['ul'][1])){
          $pkg[$pname . '-l'] = $rec['length'];
          $pkg[$pname . '-w'] = $rec['width'];
          $pkg[$pname . '-h'] = $rec['height'];
          $pkg[$pname . '-wt'] = $rec['weight'];
          $pkg[$pname . '-ul'] = $rec['ul'][1];

          if($over = $refDim[$pkg[$pname . '-ul']]){
            $pkg[$pname . '-l'] = ($tmp = $over['x_length']) ? $tmp : $pkg[$pname . '-l'];
            $pkg[$pname . '-w'] = ($tmp = $over['x_width']) ? $tmp : $pkg[$pname . '-w'];
            $pkg[$pname . '-h'] = ($tmp = $over['x_height']) ? $tmp : $pkg[$pname . '-h'];
          }
        }
      }
    }
    return $pkgs;
  }

  private function getDimensionRef()
  {
    $q = new \Oerp\Query\Basic('product.ul');
    return $q->raw()->promote('name');
  }

  private function getProducts()
  {
    $q = new \Oerp\Query\Basic(
      'product.product',
      array(array('state', '<>', 'obsolete')),
      array(
        'default_code',
      )
    );
    return $q->raw();
  }

  private function getList()
  {
    $pkgs = $this->getPackagings();
    $prods = $this->getProducts();

    $list = array();
    foreach($prods as $prod){
      $sku = $prod['default_code'];

      if(!$sku){
        continue;
      }

      if(isset($pkgs[$sku])){
        $list[$sku] = array_merge($prod, $pkgs[$sku]);
      }
      else{
        $list[$sku] = $prod;
      }
    }

    \Oerp\_Array\Command\sortSkuByKey::sort($list);
    return $list;
  }

  private function getHds()
  {
    return array(
      'default_code' => 'SKU',
      'fedex-ul' => 'Fedex Pkg',
      'fedex-l' => 'Fedex L',
      'fedex-w' => 'Fedex W',
      'fedex-h' => 'Fedex H',
      'fedex-wt' => 'Fedex Wt',
      'non-fedex-ul' => 'Non-Fedex Pkg',
      'non-fedex-l' => 'Non-Fedex L',
      'non-fedex-w' => 'Non-Fedex W',
      'non-fedex-h' => 'Non-Fedex H',
      'non-fedex-wt' => 'Non-Fedex Wt',
    );
  }

  public function content()
  {
    $t = new \Gulei\Table\Simple($this->getHds(), $this->getList());
    return $t->theme();
  }
}