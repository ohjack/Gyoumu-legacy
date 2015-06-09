<?php

require_once './includes/bootstrap.inc';
$_SERVER['REQUEST_METHOD'] = 'get';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

module_load_include('inc', 'GuLib', '/include/GuForm');

class GuFormTest extends PHPUnit_Framework_TestCase
{
  public function testCreate()
  {
    $arch = <<<XML
<form>
  <field name="name" type="textfield"/>
</form>
XML;

    $Form = new GuForm($arch);
    $this->assertNotEmpty($Form);
  }

  public function testGetFormElesTextarea()
  {
    $arch = <<<XML
<form>
  <field name="textarea" type="textarea"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'textarea',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'textarea',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="textarea" gu_name="textarea" anchor="">',
      '#suffix' => '</div>',
    );

    $this->assertEquals($exp, $eles['textarea']);

    $f = new GuForm($arch, array('textarea' => 'TEXTAREA'));
    $eles = $f->getFormEles();
    $exp['#default_value'] = 'TEXTAREA';
    $this->assertEquals($exp, $eles['textarea']);
  }

  public function testGetFormElesTextfield()
  {
    $arch = <<<XML
<form>
  <field name="textfield" type="textfield"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'textfield',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'textfield',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="textfield" gu_name="textfield" anchor="">',
      '#suffix' => '</div>',
      '#size' => '',
    );

    $this->assertEquals($exp, $eles['textfield']);

    $f = new GuForm($arch, array('textfield' => 'TEXTFIELD'));
    $eles = $f->getFormEles();
    $exp['#default_value'] = 'TEXTFIELD';
    $this->assertEquals($exp, $eles['textfield']);
  }

  public function testGetFormElesHidden()
  {
    $arch = <<<XML
<form>
  <field name="hidden" type="hidden" value="HIDDEN"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'hidden',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'hidden',
      '#default_value' => 'HIDDEN',
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="hidden" gu_name="hidden" anchor="">',
      '#suffix' => '</div>',
      '#value' => 'HIDDEN',
    );

    $this->assertEquals($exp, $eles['hidden']);
  }

  public function testGetFormElesFile()
  {
    $arch = <<<XML
<form>
  <field name="file" type="file"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'file',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'file',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="file" gu_name="file" anchor="">',
      '#suffix' => '</div>',
      '#size' => '',
    );

    $this->assertEquals($exp, $eles['file']);
  }

  public function testGetFormElesMarkup()
  {
    $arch = <<<XML
<form>
  <field name="markup" type="markup">
    <value>MARKUP</value>
  </field>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'markup',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'markup',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="markup" gu_name="markup" anchor="">',
      '#suffix' => '</div>',
      '#value' => 'MARKUP',
    );

    $this->assertEquals($exp, $eles['markup']);
  }

  public function testGetFormElesButton()
  {
    $arch = <<<XML
<form>
  <field name="button" string="Button" type="button"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'Button',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'button',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="button" gu_name="button" anchor="">',
      '#suffix' => '</div>',
      '#value' => 'Button',
      '#name' => 'button',
    );

    $this->assertEquals($exp, $eles['button']);
  }

  public function testGetFormElesSubmit()
  {
    $arch = <<<XML
<form>
  <field name="submit" string="Submit" type="submit"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'Submit',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'submit',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="submit" gu_name="submit" anchor="">',
      '#suffix' => '</div>',
      '#value' => 'Submit',
      '#name' => 'submit',
    );

    $this->assertEquals($exp, $eles['submit']);
  }

  public function testGetFormElesSelect()
  {
    $arch = <<<XML
<form>
  <field name="select" type="select">
    <option value="1">One</option>
    <option value="2">Two</option>
  </field>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'select',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'select',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="select" gu_name="select" anchor="">',
      '#suffix' => '</div>',
      '#options' =>
      array(
        1 => 'One',
        2 => 'Two',
      ),
    );

    $this->assertEquals($exp, $eles['select']);

    $f = new GuForm($arch, array('select' => '1'));
    $eles = $f->getFormEles();
    $exp['#default_value'] = '1';
    $this->assertEquals($exp, $eles['select']);
  }

  public function testGetFormElesCheckboxes()
  {
    $arch = <<<XML
<form>
  <field name="checkboxes" type="checkboxes">
    <option value="1">One</option>
    <option value="2">Two</option>
  </field>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'checkboxes',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'checkboxes',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="checkboxes" gu_name="checkboxes" anchor="">',
      '#suffix' => '</div>',
      '#options' =>
      array(
        1 => 'One',
        2 => 'Two',
      ),
    );

    $this->assertEquals($exp, $eles['checkboxes']);

    $f = new GuForm($arch, array('checkboxes' => array('1', '2')));
    $eles = $f->getFormEles();
    $exp['#default_value'] = array('1', '2');
    $this->assertEquals($exp, $eles['checkboxes']);
  }

  public function testGetFormElesGuCsv()
  {
    $arch = <<<XML
<form>
  <field name="gucsv" type="GuCsv"/>
</form>
XML;

    $f = new GuForm($arch);
    $eles = $f->getFormEles();
    $exp = array(
      '#title' => 'gucsv',
      '#required' => NULL,
      '#description' => NULL,
      '#type' => 'GuCsv',
      '#default_value' => NULL,
      '#attributes' =>
      array(
      ),
      '#ahah' =>
      array(
      ),
      '#prefix' => '<div gu_type="GuCsv" gu_name="gucsv" anchor="">',
      '#suffix' => '</div>',
      '#arch' => '',
    );

    $this->assertEquals($exp, $eles['gucsv']);
  }
}

?>