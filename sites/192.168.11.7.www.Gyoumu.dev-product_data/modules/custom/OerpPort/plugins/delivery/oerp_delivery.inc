<?php


class OerpDeliveryModel
{
  static private $DeliveryOrderCreate;
  
  public function getEles_DeliveryOrderCreate(){
    if(self::$DeliveryOrderCreate){
      return self::$DeliveryOrderCreate->getFormEntry();
    }
    else{
      $arch = file_get_contents(
        drupal_get_path('module', 'oerp_delivery') . '/xml/DeliveryOrderCreate.xml');
        
      self::$DeliveryOrderCreate = new \Oerp\Arch\Form($arch);
      return self::$DeliveryOrderCreate->getFormEntry();
    }
  }
}

?>
