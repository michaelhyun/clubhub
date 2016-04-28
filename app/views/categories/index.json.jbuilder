json.array!(@categories) do |category|
  json.extract! category, :id, :genre
  json.url category_url(category, format: :json)
end
