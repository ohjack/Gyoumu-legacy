<?php
use Gulei as G;
require_once __DIR__ . '/../include/processor.inc';

class GuModsTest extends PHPUnit_Framework_TestCase
{
  private $general_exp;

  private function createArchProcessor()
  {
    $arch = <<<XML
<tree>
  <field name="f1" string="Field 1"/>
</tree>
XML;

    $p =  new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\ProviderArch($arch))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize());

    $this->general_exp = array(
      array(
        'data' => array(
          'f1' => array(
            'data' => 'Field 1',
            'gu_name' => 'f1',
            'rowspan' => 1,
          )
        )
      )
    );

    return $p;
  }

  public function testInKeyDomain()
  {
    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, null));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), true);


    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, 'key'));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), false);


    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, array('key')));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), false);


    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, array('in' => array('key'))));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), false);


    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, array('out' => array('nokey'))));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), false);


    $stub = $this->getMockForAbstractClass(
      'Gulei\AbstractModifier', array(null, null, array('in' => array('key'), 'out' => array('nokey'))));

    $this->assertEquals($stub->isInKeyDomain('key'), true);
    $this->assertEquals($stub->isInKeyDomain('nokey'), false);
    $this->assertEquals($stub->isInKeyDomain('others'), false);
  }

  public function testGuRowModTagize()
  {
    $arch = <<<XML
<tree>
  <field name="f1" string="Field 1"/>
</tree>
XML;

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\\Gulei\Provider\Lambda(array(array())))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize())
    ;

    $this->assertEquals(array(array()), $p->getOutput());
    

    $p =  new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\ProviderArch($arch))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize());

    $exp = array(
      array(
        'data' => array(
          'f1' => array(
            'data' => 'Field 1',
            'gu_name' => 'f1',
            'rowspan' => 1,
          )
        )
      )
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuModWrapGuValue()
  {
    $arch = <<<XML
<tree>
  <param name="escape">1</param>
  <field name="f"/>
</tree>
XML;

    $recs = array(
      array('f' => ''),
      array('f' => '1'),
      array('f' => '<span class="gu_value">2</span>'),
      array('f' => '<pre>3</pre>'),
      array('f' => '<span class="gu_value"><pre>4</pre></span>'),
      array('f' => '<span><span><span class="gu_value">5</span></span></span>'),
      array('f' => '<span><span><span class="gu_value"><span>5</span></span></span></span>')
    );

    $exp = array(
      array('f' => '<span class="gu_value"></span>'),
      array('f' => '<span class="gu_value">1</span>'),
      array('f' => '<span class="gu_value">2</span>'),
      array('f' => '<span class="gu_value">&lt;pre&gt;3&lt;/pre&gt;</span>'),
      array('f' => '<span class="gu_value">&lt;pre&gt;4&lt;/pre&gt;</span>'),
      array('f' => '<span><span><span class="gu_value">5</span></span></span>'),
      array('f' => '<span><span><span class="gu_value">&lt;span&gt;5&lt;/span&gt;</span></span></span>')
    );

    $p = new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\\Gulei\Provider\Lambda($recs))
        ->applyArchReference($arch)
        ->addModifier(new G\ModEscapeXml())
        ->addModifier(new G\ModWrapValue());

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuModEscapeXml()
  {
    $arch = <<<XML
<tree>
  <field name="f"/>
</tree>
XML;

    $recs = array(
      array('f' => '<a>b&c</a>'),
    );

    $exp = array(
      array('f' => '<span class="gu_value">&lt;a&gt;b&amp;c&lt;/a&gt;</span>')
    );

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\\Gulei\Provider\Lambda($recs))
        ->applyArchReference($arch)
        ->addReference(array('_param' => array('escape' => true)))
        ->addModifier(new G\ModEscapeXml())
    ;

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuModPre()
  {
    // Do nothing because no param specified
    $p = $this->createArchProcessor();
    $p->addModifier(new G\ModPre());

    $exp = $this->general_exp;
    $this->assertEquals($exp, $p->getOutput());

    // Global param
    $newRef = array(
      '_param' => array('pre' => true)
    );

    $p = $this->createArchProcessor();
    $p->addReference($newRef)
        ->addModifier(new G\ModPre('data', 'data'));

    $exp = $this->general_exp;
    $exp[0]['data']['f1']['data'] = '<pre><span class="gu_value">Field 1</span></pre>';
    $this->assertEquals($exp, $p->getOutput());

    // Field param
    $newRef = array(
      'f1' => array('_param' => array('pre' => true))
    );

    $p = $this->createArchProcessor();
    $p->addReference($newRef)
        ->addModifier(new G\ModPre('data', 'data'));

    $exp = $this->general_exp;
    $exp[0]['data']['f1']['data'] = '<pre><span class="gu_value">Field 1</span></pre>';
    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuModNl2br()
  {
    $data = array(
      0 =>
      array(
        'data' => array(
          'fld' => array(
            'data' => "<span class=\"gu_value\">Line1\nLine2</span>"
          ),
        ),
      ),
      1 =>
      array(
        'data' => array(
          'fld' => array(
            'data' => "Line1\nLine2"
          ),
        ),
      ),
    );

    $exp[0]['data']['fld']['data'] = "<span class=\"gu_value\">Line1<br />\nLine2</span>";
    $exp[1]['data']['fld']['data'] = "<span class=\"gu_value\">Line1<br />\nLine2</span>";

    // No param
    $p = new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\\Gulei\Provider\Lambda($data))
        ->addModifier(new G\ModNl2br('data', 'data'));

    $this->assertEquals($data, $p->getOutput());

    // Global
    $p->clearReference()
        ->addReference(array('_param' => array('nl2br' => true)));

    $this->assertEquals($exp, $p->getOutput());

    // Global with exception
    $newRef = array(
      '_param' => array('nl2br' => true),
      'fld' => array('_param' => array('no-nl2br' => true))
    );

    $p->clearReference()
        ->addReference($newRef);

    $this->assertEquals($data, $p->getOutput());

    // Field param
    $newRef = array(
      'fld' => array('_param' => array('nl2br' => true))
    );

    $p->clearReference()
        ->addReference($newRef);

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuTagModHide()
  {
    $arch = <<<XML
<tree>
  <field name="f">
    <param name="hide">1</param>
  </field>
</tree>
XML;

    $recs = array(
      array('f' => '1')
    );

    $exp = array(
      array(
        'data' => array(
          'f' => array(
            'data' => '1',
            'gu_name' => 'f',
            'rowspan' => 1,
            'style' => 'display:none;',
          )
        )
      )
    );


    $p = new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\\Gulei\Provider\Lambda($recs))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize())
        ->addModifier(new G\TagModHide());

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuTagAppendEmptyRow()
  {
    $arch = <<<XML
<tree>
  <field name="f1"/>
  <field name="f2">
    <param name="hide">1</param>
  </field>
</tree>
XML;

    $Arch = new \Gulei\Arch\Element($arch);
    $hds = $Arch->extractAttr('string', 'name');

    // Empty
    $recs = array(array());

    $exp = array(
      array(
        'data' => array(''),
        'empty' => 1,
      )
    );

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\ProviderCompose($recs, $hds))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize())
        ->addModifier(new G\ModAppendEmpty())
    ;

    $this->assertEquals($exp, $p->getOutput());

    // Not empty
    $recs = array(
      array(
        'f1' => '1',
        'f2' => '2',
      )
    );

    $exp = array(
      array(
        'data' => array(
          'f1' => array(
            'data' => '1',
            'gu_name' => 'f1',
            'rowspan' => 1,
          ),
          'f2' => array(
            'data' => '2',
            'gu_name' => 'f2',
            'rowspan' => 1,
          )
        )
      )
    );

    $p = new \Gulei\Processor\Basic();
    $p->setDataProvider(new G\ProviderCompose($recs, $hds))
        ->applyArchReference($arch)
        ->addModifier(new G\ModTagize())
        ->addModifier(new G\ModAppendEmpty());

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuTagModDitto()
  {
    $hds = array(
      'f1' => 'F1',
    );

    $recs = array(
      array('f1' => 'v1'),
      array('f1' => '_DITTO_'),
      array('f1' => '_DITTO_'),
      array('f1' => 'v2'),
      array('f1' => '_DITTO_'),
      array('f1' => 'v3'),
    );

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\ProviderCompose($recs, $hds))
        ->addReference(array('_param' => array('copy_method' => 'ditto')))
        ->addModifier(new G\ModTagize())
        ->addModifier(new G\TagModCopyDitto())
    ;

    $exp = array(
      array(
        'data' => array(
          'f1' => array(
            'data' => 'v1',
            'gu_name' => 'f1',
            'rowspan' => 3,
          ),
        ),
        'class' => 'merge-odd'
      ),
      array(
        'data' => array(),
        'merged' => '1',
        'class' => 'merge-odd',
      ),
      array(
        'data' => array(),
        'merged' => '1',
        'class' => 'merge-odd',
      ),
      array(
        'data' => array(
          'f1' => array(
            'data' => 'v2',
            'gu_name' => 'f1',
            'rowspan' => 2,
          )
        ),
        'class' => 'merge-even',
      ),
      array(
        'data' => array(),
        'merged' => '1',
        'class' => 'merge-even',
      ),
      array(
        'data' => array(
          'f1' => array(
            'data' => 'v3',
            'gu_name' => 'f1',
            'rowspan' => 1,
          )
        ),
        'class' => 'merge-odd',
      ),
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuTagModMerge()
  {
    $hds = array(
      'f1' => 'F1',
      'f2' => 'F2',
    );

    $recs = array(
      array('f1' => 'v1', 'f2' => 'v1.1'),
      array('f1' => '_DITTO_', 'f2' => 'v1.2'),
      array('f1' => '_DITTO_', 'f2' => 'v1.3'),
      array('f1' => 'v2', 'f2' => 'v2.1'),
      array('f1' => '_DITTO_', 'f2' => 'v2.2'),
      array('f1' => 'v3', 'f2' => 'v3.1'),
    );

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\ProviderCompose($recs, $hds))
        ->addReference(array('_param' => array('copy_method' => 'merge')))
        ->addModifier(new G\ModCopyMerge())
    ;

    $exp = array (
      0 =>
      array (
        'f1' => 'v1',
        'f2' => 'v1.1
v1.2
v1.3',
      ),
      1 =>
      array (
        'f1' => 'v2',
        'f2' => 'v2.1
v2.2',
      ),
      2 =>
      array (
        'f1' => 'v3',
        'f2' => 'v3.1',
      ),
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testGuTagModAddCountColumn()
  {
    $arch = <<<XML
<tree>
  <field name="f"/>
</tree>
XML;

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\ProviderArch($arch))
        ->addReference(array('_param' => array('count' => true)))
        ->addModifier(new G\ModTagize())
        ->addModifier(new G\TagModAddCountColumn())
    ;

    $exp = array(
      array(
        'data' => array(
          '%count' => array(
            'data' => '#',
            'gu_name' => '%count',
            'rowspan' => 1,
          ),
          'f' => array(
            'data' => 'f',
            'gu_name' => 'f',
            'rowspan' => 1,
          )
        )
      )
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testModUsort()
  {
    $recs = array(
      array('n' => 3),
      array('n' => 2),
      array('n' => 5),
      array('n' => 4),
      array('n' => 1),
    );

    $funcSort = create_function('$row1,$row2', 'return strcmp($row1["n"], $row2["n"]);');

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\\Gulei\Provider\Lambda($recs))
        ->addReference(array('_param' => array('sort' => $funcSort)))
        ->addModifier(new G\ModUsort())
    ;

    $exp = array(
      array('n' => 1),
      array('n' => 2),
      array('n' => 3),
      array('n' => 4),
      array('n' => 5),
    );

    $this->assertEquals($exp, $p->getOutput());
  }

  public function testModAddCount()
  {
    $recs = array(
      array('n' => '1'),
      array('n' => '_DITTO_'),
      array('n' => '3'),
    );

    $exp = array(
      array('n' => '1', '%count' => 1),
      array('n' => '_DITTO_', '%count' => '_DITTO_'),
      array('n' => '3', '%count' => 2),
    );

    $p = new \Gulei\Processor\Basic();
    $p
        ->setDataProvider(new G\\Gulei\Provider\Lambda($recs))
        ->addReference(array('_param' => array('count' => true, 'copy_method' => 'ditto')))
        ->addModifier(new G\ModAddCount)
    ;

    $this->assertEquals($exp, $p->getOutput());
  }
}