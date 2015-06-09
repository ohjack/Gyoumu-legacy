$(document).ready ->
  load_sku = new LoadSku()

  $('#edit-load-sku-search-sku').click ->
    load_sku.onClick('SearchSku')

  $('#edit-load-sku-search-pmgr').click ->
    load_sku.onClick('SearchPmgr')

  $rs = $('#OutputStatistics-result')
  if ($rs.length)
    $(window).scrollTop($rs.position().top)


class LoadSku
  getSkuInputEle: =>
    return $('#edit-skus')

  setSkuInputText: (text)=>
    $ele = @.getSkuInputEle()

    if text == false
      $ele.text('Loading...').attr('disabled', 'disabled')
    else
      $ele.text(text).removeAttr('disabled')

  getLoadSkuEle: =>
    return $('#edit-load-sku')

  sendPost: (send, func)=>
    $.post('?q=oerp/query/js', {send: send}, func)

  getSend: (type)=>
    key = @.getLoadSkuEle().val()
    tpl = [
      'product.product'
      [['state', '<>', 'obsolete']]
      ['default_code']
    ]

    switch type
      when 'SearchSku'
        tpl[1].push(
          ['default_code', 'ilike', key]
        )

      when 'SearchPmgr'
        tpl[1].push(
          ['product_manager', 'ilike', key]
        )
    tpl

  onClick: (type)=>
    @.setSkuInputText(false)
    send = JSON.stringify(@.getSend(type))

    func = (data)=>
      resp = JSON.parse(data)

      if resp?
        skus = ''
        for rec in resp
          skus += rec.default_code + "\n"

        @.setSkuInputText(skus)

      else
        @.setSkuInputText('')

    @.sendPost(send, func)
    false