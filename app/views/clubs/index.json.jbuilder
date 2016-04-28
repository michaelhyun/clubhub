json.array!(@clubs) do |club|
  json.extract! club, :id, :Name, :Admin, :NumberOfMembers, :category
  json.url club_url(club, format: :json)
end
