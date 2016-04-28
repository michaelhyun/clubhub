json.array!(@orgpages) do |orgpage|
  json.extract! orgpage, :id, :title, :body
  json.url orgpage_url(orgpage, format: :json)
end
