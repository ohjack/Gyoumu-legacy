<?php
namespace Yauc\Page\Listings\Active\Callee;

class Download extends \Gulei\Page\Form\Calee\_Abstract
{
  protected function getClassname()
  {
    return __CLASS__;
  }

  public function execute($form, &$form_state)
  {
    $vals = $this->FormPage->getValues();
    $prov = new \Yauc\Provider\Listings\Active($vals['sellerId']);
    $Recs = new \Gulei\_Array\Basic($prov->getData());

    $mapping = <<<XML
<tree>
  <field name="SKU" from="sku"/>
  <field name="Title" from="title"/>
  <field name="Init.Price" from="init_price"/>
  <field name="Cur.Price" from="cur_price"/>
  <field name="S.Fee" from="shipping_fee"/>
  <field name="Carrier" from="carrier"/>
</tree>
XML;

    $mapped = $Recs->mapping($mapping);
    array_unshift($mapped, array_keys($mapped[0]));

    $Recs = new \Gulei\_Array\Basic($mapped);
    $csv_src = $Recs->dumpCsv();

    $path = sprintf('%s/%s_%s_%s.csv', file_directory_temp(), 'ActivePricelist', $vals['sellerId'], date('Ymd'));
    $fp = '.' . file_save_data($csv_src, $path);
    $form_state['storage']['redirect'] = $fp;
  }
}