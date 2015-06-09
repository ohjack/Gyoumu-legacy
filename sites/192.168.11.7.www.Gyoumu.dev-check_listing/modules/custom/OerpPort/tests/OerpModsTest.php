<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

class OerpModsTest extends PHPUnit_Framework_TestCase
{
  public function testOerpTagModAddOpColumn()
  {
    $recs = array(
      array('f' => 'F')
    );

    $p = new GuProcessor();
    $p
        ->setDataProvider(new ProviderLambda($recs))
        ->addModifier(new ModTagize())
        ->addModifier(new OerpTagModAddOpColumn())
    ;

    $exp = array(
      array(
        'data' => array(
          '#op' => array(
            'data' => '',
            'class' => 'oerp-op',
          ),
          'f' => array(
            'data' => 'F',
            'gu_name' => 'f',
            'rowspan' => 1,
          )
        )
      )
    );

    $this->assertEquals($exp, $p->getOutput());

    $param = array('oerp:op-type' => 'edit');
    $p->addReference(array('_param' => $param));

    $srcEdit = <<<XML
<div class="oerp-button oerp-btn-add ui-state-default ui-corner-all">
  <span class="oerp-icon ui-icon ui-icon-document"></span>
</div>
XML;

    $exp = array(
      array(
        'data' => array(
          '#op' => array(
            'data' => $srcEdit,
            'class' => 'oerp-op',
          ),
          'f' => array(
            'data' => 'F',
            'gu_name' => 'f',
            'rowspan' => 1,
          )
        )
      )
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testOerpTagModUseParamCaption()
  {
    $recs = array(
      array('f' => 'F')
    );

    $param = array(
      'fields' => array(
        'f' => array('string' => 'Field')
      )
    );

    $p = new GuProcessor();
    $p
        ->setDataProvider(new ProviderLambda($recs))
        ->addReference(array('_param' => $param))
        ->addModifier(new ModTagize())
        ->addModifier(new OerpTagModUseParamString())
    ;

    $exp = array(
      array(
        'data' => array(
          'f' => array(
            'data' => '<span class="gu_value">Field</span>',
            'gu_name' => 'f',
            'rowspan' => 1,
          )
        )
      )
    );

    $this->assertEquals($exp, $p->getOutput());
  }
}