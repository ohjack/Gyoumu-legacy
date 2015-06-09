<?php
namespace Oerp\Gallery\File\Uploader;

class Amazon extends \Oerp\Gallery\File\Uploader\_Abstract {
  protected $ups = array();

  protected function parseFname($up) {
    preg_match('/(.*)-(\d)\.jpg/', $up, $m);

    return array(
      'fname' => $m[0],
      'SKU' => $m[1],
      'ind' => $m[2],
    );
  }

  protected function getMsg($up) {
    static $MessageID = 0;
    $MessageID++;

    $m = $this->parseFname($up);

    if ($m['ind'] > 1) {
      $ImageType = 'PT' . ($m['ind'] - 1);
    }
    else {
      $ImageType = 'Main';
    }

    if ($this->getLocalFileSize($up) > 300) {
      $OperationType = 'Update';

      return <<<XML
  <Message>
    <MessageID>$MessageID</MessageID>
    <OperationType>$OperationType</OperationType>

    <ProductImage>
      <SKU>{$m['SKU']}</SKU>
      <ImageType>$ImageType</ImageType>
      <ImageLocation>http://picturemaster.info/ProductPics/Amazon_titosoy/{$m['fname']}</ImageLocation>
    </ProductImage>
  </Message>
XML;
    }
    else {
      $OperationType = 'Delete';

      return <<<XML
  <Message>
    <MessageID>$MessageID</MessageID>
    <OperationType>$OperationType</OperationType>

    <ProductImage>
      <SKU>{$m['SKU']}</SKU>
      <ImageType>$ImageType</ImageType>
    </ProductImage>
  </Message>
XML;
    }
  }

  function __destruct() {
    $msgs = '';

    if (!$this->ups) {
      return;
    }

    foreach ($this->ups as $up) {
      for ($c = 1; $c <= 3; $c++) {
        $msgs .= $this->getMsg($up . '-' . $c . '.jpg');
      }
    }

    $Call = new \Amapi\API\SubmitFeed();
    $resp = $Call->getResponse(array(
      'FeedType' => '_POST_PRODUCT_IMAGE_DATA_',
      'Messages' => $msgs,
    ));
    print_r($resp);
  }

  protected function getAccount() {
    return 'Amazon_titosoy';
  }

  protected function isAvailable() {
    return TRUE;
  }

  protected function isOverLimit($up) {
    return FALSE;
  }

  protected function isSkip($up) {
    $m = $this->parseFname($up);
    return ($m['ind'] > 0) ? FALSE : TRUE;
  }

  protected function upload($up) {
    $toks = $this->parseFname($up);
    $sku = $toks['SKU'];

    if (!$this->up[$sku]) {
      $this->ups[$sku] = $sku;
    }
    return TRUE;
  }
}